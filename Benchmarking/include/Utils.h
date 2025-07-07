/**
 * Helper functions 
 */
#ifndef UTILS_H
#define UTILS_H

#include <vector>
#include <iostream>

using namespace std;

extern "C"
{
  #include "./mt19937ar.h"

}
// generate angles 
void angles_generator(int qubits, int depth, vector<double> &angles_array);

#endif