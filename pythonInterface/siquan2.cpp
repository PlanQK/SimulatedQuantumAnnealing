/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Copyright 2020, d-fine GmbH, Daniel Herr                                    *
 *                                                                             *
 * Licensed under the Apache License, Version 2.0 (the "License");             *
 * you may not use this file except in compliance with the License.            *
 * You may obtain a copy of the License at                                     *
 *                                                                             *
 *   http://www.apache.org/licenses/LICENSE-2.0                                *
 *                                                                             *
 * Unless required by applicable law or agreed to in writing, software         *
 * distributed under the License is distributed on an "AS IS" BASIS,           *
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.    *
 * See the License for the specific language governing permissions and         *
 * limitations under the License.                                              *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * ** */

/** \file
 * \brief A small Python Interface
 * \author
 * Year      |  Name
 * ----      | -----
 * 2020      | d-fine GmbH, Daniel Herr
 * \copyright  Apache License, Version 2.0 */

#include <siquan/algo/algo.hpp>
#include <siquan/base.hpp>
#include <siquan/connect/connect.hpp>
#include <siquan/helper.hpp>
#include <siquan/scheduler/scheduler.hpp>
#include <siquan/state/simple.hpp>
#include <siquan/state/trotter.hpp>
#include <siquan/types/types.hpp>

#include <fsc/ArgParser.hpp>
#include <fsc/profiler.hpp>

#include <iostream>
#include <map>
#include <vector>

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
namespace py = pybind11;
namespace siquan
{ // documented in base.hpp

    struct type_carrier
    {
        // connect
        using user_type = uint32_t;
        using vert_type = uint32_t;

        using weight_type = double;
        using constr_type = double;

        using size_type = size_t;
        using vert_vec_type = std::vector<vert_type>;

        using connect_type =
            siquan::types::connect_type<weight_type, vert_vec_type>;
        using connect_vec_type = std::vector<connect_type>;

        // state
        using state_type = std::vector<siquan::types::DynamicTrotterLine>;

        // scheduler
        using temp_type = double;
        using magn_type = double;
    };
    using grid_type =
        compose<type_carrier, connect::basic, connect::readPythonStructure, connect::remap,
                connect::merge_equal,

                scheduler::sim_step, scheduler::piecewise_multi_T,
                scheduler::piecewise_multi_H,

                state::trotter,

                connect::normalize_weight, algo::simulated_quantum_anealing,
                connect::unnormalize_weight, algo::analyze_energy_trotter,
                algo::best_trotter,

                algo::analyze_energy,

                connect::unmap>;
} // end namespace siquan

using namespace siquan;

using problemType = std::vector<std::pair<double, std::vector<uint>>>;

class Interface
{
public:
    Interface() : ap()
    {
    }
    void setTSchedule(std::string T)
    {
        ap.def("T", T);
    }
    void setHSchedule(std::string H)
    {
        ap.def("H", H);
    }
    void setSeed(uint seed)
    {
        ap.def("seed", seed);
    }
    void setSteps(uint steps)
    {
        ap.def("steps", steps);
    }
    void setTrotterSlices(uint slices)
    {
        ap.def("nt", slices);
    }
    std::map<std::string, std::string> minimize(
        const std::vector<std::pair<type_carrier::weight_type, type_carrier::vert_vec_type>> &problem,
        uint32_t n_vert)
    {
        ap.def("seed", "0");
        ap.def("steps", "1000");
        ap.def("remap", "sorted,fill,0");
        ap.def("T", "[0.01,0.01]");
        ap.def("H", "[10,iF,0.01]");
        ap.def("nt", "100");

        grid_type::param p;
        p.from_map(ap.n_args());

        std::map<std::string, std::string> m;

        fsc::rdtsc_timer t;

        grid_type g(p);

        g.setProblem(problem, n_vert);
        g.init();

        t.start();
        g.update();
        while (not g.stop())
        {
            g.step();
            g.advance(1);
            g.update();
        }
        g.finish();

        t.stop();

        p.to_map(m);
        siquan::to_map(m, t);
        g.to_map(m);
        return m;
    }

private:
    fsc::ArgParserTpl<std::string> ap;
};

PYBIND11_MODULE(siquan, m)
{
    py::class_<Interface>(m, "DTSQA")
        .def(py::init())
        .def("setTSchedule", &Interface::setTSchedule,
             "Set the temperature schedule.\n\
Example of a constant temperature schedule through the annealing run:\n\
>>> dtsqa.setTSchedule(\"[[0.01,0.01]]\")\n")
        .def("setHSchedule", &Interface::setHSchedule,
             "Set the transversal field strength throughout the annealing run.\
Example of a linear decreasing transversal field:\
>>> dtsqa.setHSchedule(\"[10,iF,0.01]\")")
        .def("setSeed", &Interface::setSeed,
             "Set the seed for the random number generator.\
>>> dtsqa.setSeed(0)")
        .def("setSteps", &Interface::setSteps,
             "Set how many annealing steps are taken.\
N steps result in N sweeps across the problem and N changes in the external field/temperature.\
>>> dtsqa.setSteps(1000)")
        .def("setTrotterSlices", &Interface::setTrotterSlices,
             "This sets the number of discrete imaginary-time slices in the simulation.\
A large number is more physical but slower for the simulation.\
>>> setTrotterSlices(100)")
        .def("minimize", &Interface::minimize,
             "Minimize the given problem description. The format needs to be a list of couplings,\n\
where each coupling is described by a tuple of the coupling strength and the qubits.\n\
Additionally, the number of qubits needs to be supplied.\n\
The labels for the qubits need to be integers and in [0, N-1]\n\
Example of a single spin (id=0) with a coupling strength of 1.\n\
>>> dtsqa.minimize([(1., [0])], 1)\n\
Energy: -1.0\n\\n\
Example of a 3 qubit interaction: E = x_1*x_2*x_3 - x_1 - x_2\n\
>>> dtsqa.minimize([(1., [0,1,2]), (-1., [0]), (-1.,[1])])\n\
Energy: -3.0\n\\n\
One thing to note: The qubits have the states -1, 1 (for the energy calculation).\n\
But the output will be {0,1} where the -1 qubit state is substituted for a 0.\n",
             py::arg("problem"), py::arg("num_qubits"));
};