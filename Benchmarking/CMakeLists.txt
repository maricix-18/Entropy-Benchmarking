cmake_minimum_required(VERSION 3.10)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(entropy_benchmarking
  DESCRIPTION "Entropy Benchmarking using QuEST"
  LANGUAGES CXX C
  )

set(MAIN_SRC "${CMAKE_SOURCE_DIR}/src")
set(MAIN_H "${CMAKE_SOURCE_DIR}/include")

set(QuEST "${CMAKE_SOURCE_DIR}/../QuEST")

# quest header file directory, build dir
set(QuEST_build "${QuEST}/build/include")
set(QuEST_lib "${QuEST}/build")
set(QuEST_q "${QuEST}")

# executable
add_executable(entropy_benchmarking 
 src/main.cc
 src/Backend.cc
 src/Protocol.cc
 src/mt19937ar.c
 src/Backends/Simulator.cc
 src/Protocols/DensityMatrix.cc
 src/Protocols/ClassicalShadows.cc
 src/Protocols/Swap.cc
)

# header files
target_include_directories(entropy_benchmarking PUBLIC 
"${CMAKE_SOURCE_DIR}/include" # this project header files 
"${CMAKE_SOURCE_DIR}/include/Backends" # backends header files
"${CMAKE_SOURCE_DIR}/include/Protocols" # protocol header files
"${QuEST_lib}"
"${QuEST}" 
"${QuEST_build}" 
"${QuEST_q}"
"../json/include"
"../eigen"
)

#link QuEST library
#target_link_libraries(entropy_benchmarking "${QuEST_lib}/QuEST.lib"  Eigen3::Eigen)

target_link_libraries(entropy_benchmarking "${QuEST_lib}/libQuEST.so")