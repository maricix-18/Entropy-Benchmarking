cmake_minimum_required(VERSION 3.28)

project(entropy_benchmarking
  DESCRIPTION "Entropy Benchmarking using QuEST"
  LANGUAGES CXX C
  )

set(MAIN_DIR "${CMAKE_SOURCE_DIR}/src")
set(MAIN_H "${CMAKE_SOURCE_DIR}/include")

# quest header file directory, build dir
set(QuEST_H "${QuEST_LIBRARY_DIR}/include")
set(QuEST_DIR "${CMAKE_SOURCE_DIR}/../QuEST")
set(QuEST_LIBRARY_DIR "${CMAKE_SOURCE_DIR}/../QuEST/build")

# link libQuest.so
link_directories(entropy_benchmarking PUBLIC 
${QuEST_LIBRARY_DIR}
)

# executable
add_executable(entropy_benchmarking 
${MAIN_DIR}/main.cc
${MAIN_DIR}/Layer.cc
${MAIN_DIR}/Utils.cc
${MAIN_DIR}/mt19937ar.c
)

# header files
target_include_directories(entropy_benchmarking PUBLIC 
"${QuEST_H}" # other quest header files
"${QuEST_LIBRARY_DIR}/include" # quest.h
"${CMAKE_SOURCE_DIR}/include" # this project header files 
"${QuEST_DIR}"
)

# https://eigen.tuxfamily.org/index.php?title=Main_Page#Overview 
find_package(Eigen3 3.3 REQUIRED NO_MODULE)

#link QuEST library
target_link_libraries(entropy_benchmarking QuEST Eigen3::Eigen)