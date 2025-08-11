# Run script

#!/bin/bash

# run executable
# ./Benchmarking/build/entropy_benchmarking

cd Bechmarking
rmdir /s /q build

mkdir build
cd build
cmake ..
msbuild entropy_benchmarking.sln

Debug\entropy_benchmarking.exe
# run on windows
# cmake ..
# msbuild .sln file
# add to terminal: set PATH=C:\Users\maria\Desktop\Entropy_Benchmark_quest\Entropy-Benchmarking\QuEST\build;%PATH%
# then .exe in Debug
