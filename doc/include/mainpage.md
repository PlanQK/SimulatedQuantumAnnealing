# SiQuAn Overview

The SiQuAn libaray provides a fast implementation for solving spin glasses (z Operators only) with
higher order z-Interactions. Given \f$N\f$ spins (\f$S=\frac{1}{2}\f$) with interactions
\f$J_{i,j,k,...}\f$ (\f$J_i\f$ would be an extern field coupling, \f$J_{i,j}\f$ a two-body interaction, \f$J_{i,j,k}\f$ a three-body interaction, ...)
the Hamiltonian that will be optimized is:
\f{eqnarray*}{
    H=-\sum _i J_i \sigma _i^z-\sum _{i,j} J_{i,j} \sigma _i^z \sigma _j^z -\sum _{i,j,k} J_{i,j,k} \sigma _i^z \sigma_j^z \sigma_k^z  -\ldots
\f}.

## Performance

More information later. Tested against exact solvers up to 200 spins with order
of 10k interactions and matched optimal result in short time (<10min) on one core.

## Theoretical Background

The exact theoretical background and algorithm will be provided at a later point.
We use [suzuki-trotter](https://en.wikipedia.org/wiki/Time-evolving_block_decimation#The_Suzuki-Trotter_expansion) to map the quantum model to an anisotropic classical
model, which we then optimize with [metropolis-montecarlo](https://en.wikipedia.org/wiki/Metropolis%E2%80%93Hastings_algorithm). All references and
detailed explanations follow later.



## Technical Background

SiQuAn is written in C++ and requires full C++11 support. It compiles with
    -std=c++11
on gcc and with
    -std=c++14
on clang.


Since performance and flexibility are key in HPC science, this framework uses the static polymorphism extensively.
This is achieved with the [decorator pattern / Mixin Inheritance](https://en.wikipedia.org/wiki/Decorator_pattern#Static_Decorator_(Mixin_Inheritance)) and in rare cases with [CRTP](https://en.wikipedia.org/wiki/Curiously_recurring_template_pattern). The framework uses bit-coding for efficiency, but so far no [intrinsics](https://en.wikipedia.org/wiki/Intrinsic_function), since there was no need yet. Since we need to run the same code on millions of instances, the code is optimized for a single core, and no multicore/thread support for one large instance is available.

## Basic Demonstration

This is all the code needed to run a simulated quantum annealing run. There will
be more and better tutorials in the future.

~~~{.cpp}
    struct type_carrier {
        using user_type = uint32_t; // the type the user uses to input
        using vert_type = uint32_t; // the internal label type for the spins
        using weight_type = double; // type of interaction weight/energy

        using size_type = size_t;   // type for simulation counter
        using vert_vec_type = std::vector<vert_type>; // index vector
        using connect_type =
            siquan::types::connect_type<weight_type, vert_vec_type>;
        using connect_vec_type = std::vector<connect_type>;
        // using a vector of dynamic trotter line type as state
        using state_type = std::vector<siquan::types::DynamicTrotterLine>;

        using temp_type = double; // for the temperature scheduler
        using magn_type = double; // for the transverse field scheduler
    };

    using grid_type =
        compose<type_carrier,                     // all types for all modules
                connect::basic,                   // stores connections
                connect::read_in_txt,             // reads textfile
                connect::remap,                   // remaps the vertices
                scheduler::sim_step,              // hold a simulation counter
                                                  // and stops it
                scheduler::piecewise_linear_T,    // Temperature scheduler
                scheduler::piecewise_linear_H,    // Field scheduler
                state::trotter,                   // the spin state
                algo::simulated_quantum_anealing, // algorithm
                algo::analyze_energy_trotter,     // analyze all trotter slices
                algo::best_trotter,               // pick best
                algo::analyze_energy,             // access to energy of best
                connect::unmap                    // return state in user labels
                >;
    std::map<std::string, std::string> m;

    // Theses are just demonstration parameters
    m["file"]  = "../test/data/frust.txt";
    m["seed"]  = "0";
    m["steps"] = "1000";
    m["remap"] = "sorted,fill,0";
    m["T"]  = "[0.01,0.01]";
    m["H"]  = "[10,0.01]";
    m["nt"] = "100";

    grid_type::param p;   // set up the parameter object
    p.from_map(m);        // read in the parameters
    p.seed = 0;           // alternative setup possibility
    grid_type g(p);       // create grid_type object now that
                          // the parameters are set

    g.init();             // initializes the grid (read file, setup states)

    g.update();           // update any party that depends on the schedulers
    while(not g.stop()) { // run while no module that implements stop stops
        g.step();         // evolve the state
        g.advance(1);     // advance the schedulers
        g.update();       // update any party that depends on the schedulers
    }
    g.finish();           // post-processing (get measurable)
    p.to_map(m);          // store all results
    g.energy();           // get the minimized energy
~~~



## Details

\author
Year      | Name       
--------  | ----------
2015-2018 | ETH Zurich, D-PHYS, Mario S. Koenz
\copyright Licensed under the [Apache License](https://www.apache.org/licenses/LICENSE-2.0). See [LICENSE](../../LICENSE) for details.
