@echo off
cd Benchmarking
rmdir /s /q build

mkdir build
cd build
cmake ..
msbuild entropy_benchmarking.sln

Debug\entropy_benchmarking.exe

cd ..
cd ..
