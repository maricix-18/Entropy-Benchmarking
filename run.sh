# Run script it will rebuild only the benchmarking project
#!/bin/bash

cd Benchmarking
rm -rf build

echo "Building Benchmarking Project..."
mkdir build
cd build
cmake ..
make

export VERBOSE=1
export OMP_NUM_THREADS=52
export GOMP_CPU_AFFINITY="0-51"
export OMP_DISPLAY_ENV=verbose
export OMP_DISPLAY_AFFINITY=true

echo "VERBOSE = $VERBOSE"
echo "OMP_NUM_THREADS = $OMP_NUM_THREADS"
echo "GOMP_CPU_AFFINITY = $GOMP_CPU_AFFINITY"
echo "OMP_DISPLAY_ENV = $OMP_DISPLAY_ENV"
echo "OMP_DISPLAY_AFFINITY = $OMP_DISPLAY_AFFINITY"


./entropy_benchmarking

cd ..
cd ..
