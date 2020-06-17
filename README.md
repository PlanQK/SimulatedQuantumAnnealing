# SiQuAn Optimizer
The SiQuAn library provides a fast implementation for solving spin glasses (z-Operators only) with higher order z-Interactions. The original implementation can be found here: http://doi.org/10.5905/ethz-1007-127

## Installation
SiQuAn is written in C++ and requires full C++11 support. It compiles with -std=c++11 on gcc and with -std=c++14 on clang. Compilation requires at least CMake version 3.0. For the documentation, Doxygen is required. The compilation of the Python plugin requires pybind11. For the Python plugin, the version needs to be adjusted in the CMakeListsFolder or using the cmake variable flag: `-DPYBIND11_PYTHON_VERSION`
The dependencies for this project can be downloaded with:
```
git submodule init
git submodule update
```
Then the Optimizer can be compiled out of directory using:
```
mkdir build
cd build
cmake ..
make
```

Alternatively a docker container can be built from the main directory with:
```
docker build -t siquan .
```
The Dockerfile does not contain any Python bindings and only runs the solver natively.

## Run the optimizer from within Python

To enable the python interface, the PYTHONPATH needs to contain the compiled python interface file, eg. `build/pythonInterface`.
Then the optmizer can be imported and used:
```python
import SiQuAn
dtsqa = SiQuAn.DTSQA()
# possibility to set various parameters
dtsqa.setHSchedule("[10,iF,0]")

# first param: problem description
# second param: number of qubits
resultDict = dtsqa.minimize(
    [(1,[0]), (1.5, [0,1]), (-1., [1,2,3])],
    4)
```

## Run the binary executable
To run the binary executable under `build/sqa_direct` type:
```bash
./sqa_direct --file path/to/file
```
In the test folder there are a few sample files. Further possible parameters and their default values are:
- file: ../test/data/frust.txt
- seed: 0
- steps: 1000
- remap: sorted,fill,0
- T: [0.01,0.01]
- H: [10,iF,0.01]
- nt: 100


The schedule can be set differently (p linearly evolves from 0 to 1):
- [10,1] or [10,l,1]: linear ramp from 10 to 1
- [a,iF,b]: (a * b) / (b + (a - b) * p) This is fast at the beginning and slow towards the end
- [a,iS,b]: a + b - (a * b) / (a - (a - b) * p) This is slow at the beginning and fast towards the end
- [a,sS,b]:  a + (b - a) * p^2
- [a,sF,b]: b + (a - b) * (p-1)^2
- [10,l,2,2,l,1]: first a linear ramp from 10 to 2 and then in the same time a linear ramp from 2 to 1


## Documentation
More information about the code can be seen in the Doxygen documentation.