/** * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Copyright 2016-2018, ETH Zurich, ITP, Mario S. Koenz                        *
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
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 */

/** ****************************************************************************
 * \file
 * \brief
 * \author
 * Year      | Name
 * --------: | :------------
 * 2016      | Mario S. Koenz
 * \copyright  todo
 ******************************************************************************/

#include <siquan/algo/algo.hpp>
#include <siquan/base.hpp>
#include <siquan/connect/connect.hpp>
#include <siquan/helper.hpp>
#include <siquan/observer/observer.hpp>
#include <siquan/scheduler/scheduler.hpp>
#include <siquan/state/simple.hpp>
#include <siquan/state/trotter.hpp>
#include <siquan/types/types.hpp>

#include <fsc/ArgParser.hpp>
#include <fsc/profiler.hpp>

#include <iostream>
#include <map>
#include <vector>

namespace siquan {  // documented in base.hpp

struct type_carrier {
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
    compose<type_carrier, connect::basic, connect::read_in_txt, connect::remap,
            connect::merge_equal,

            scheduler::sim_step, scheduler::piecewise_multi_T,
            scheduler::piecewise_multi_H, observer::T_scheduler,
            observer::H_scheduler, state::trotter,

            connect::normalize_weight, algo::simulated_quantum_anealing,
            connect::unnormalize_weight,

            algo::analyze_energy_trotter, observer::trotter, algo::best_trotter,

            algo::analyze_energy,

            connect::unmap>;
}  // end namespace siquan

using namespace siquan;

int main(int argc, char* argv[]) {
    fsc::ArgParserTpl<std::string> ap(argc, argv);

    grid_type::param p;

    std::map<std::string, std::string> m;

    // set some defaults if we don't want to feed cmd args
    ap.def("file", ap.get(0, "../test/data/frust.txt"));
    ap.def("seed", "0");
    ap.def("steps", "1000");
    ap.def("remap", "sorted,fill,0");
    ap.def("T", "[0.01,0.01]");
    ap.def("H", "[10,iF,0.01]");
    ap.def("nt", "100");

    p.from_map(ap.n_args());

    fsc::rdtsc_timer t;
    t.start();

    grid_type g(p);

    g.init();

    g.update();
    while(not g.stop()) {
        g.step();
        g.advance(1);
        g.update();
    }
    g.finish();

    t.stop();

    p.to_map(m);
    siquan::to_map(m, t);
    g.to_map(m);

    for(auto const& a : m) {
        std::cout << a.first << " " << a.second << std::endl;
    }

    return 0;
}
