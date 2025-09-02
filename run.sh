# Run script

#!/bin/bash

# run executable
# ./Benchmarking/build/entropy_benchmarking

cd Bechmarking
rm -rf build

mkdir build
cd build
cmake ..
make

entropy_benchmarking

cd ..
cd ..
# run on windows
# cmake ..
# msbuild .sln file
# add to terminal: set PATH=C:\Users\maria\Desktop\Entropy_Benchmark_quest\Entropy-Benchmarking\QuEST\build;%PATH%
# then .exe in Debug
