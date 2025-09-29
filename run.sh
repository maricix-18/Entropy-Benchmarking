# Run script it will rebuild only the benchmarking project
#!/bin/bash

cd Benchmarking
rm -rf build

mkdir build
cd build
cmake ..
make

./entropy_benchmarking

cd ..
cd ..
