/**
 * Helper functions 
 */
#ifndef UTILS_H
#define UTILS_H

#include <vector>
#include <iostream>

using namespace std;

#define PI 3.14159265358979323846

extern "C"
{
  #include "./mt19937ar.h"

}
// generate angles 
void angles_generator(int qubits, int depth, vector<double> &angles_array);

// generate measurement setting
void generate_measurement_setting(vector<int> &measurement_setting, int qubits);

#endif