# Set up script
#!/bin/bash

# check if QuEST directory exists
if [ ! -d "QuEST" ]; then
    echo "Cloning QuEST V4 repository..."
    git clone https://github.com/QuEST-Kit/QuEST.git

else
    echo "QuEST directory already exists."
fi

# check if QuEST is already built
cd QuEST
if [ -d "build" ]; then
    echo "QuEST Project already built."
else
    echo "Building QuEST V4 Project..."
    mkdir build
    cd build
    cmake ..
    make 
    # out of build directory
    cd ..
fi
# out of QuEST directory
cd ..

# check if Benchmarking is built
cd Benchmarking
if [ -d "build" ]; then
    echo "Benchmarking Project already built."
    echo "Remove build."
    rm -rf build
fi

echo "Building Benchmarking Project..."
mkdir build
cd build
cmake ..
make
# out of build directory
cd ..

# out of Benchmarking directory
cd ..
