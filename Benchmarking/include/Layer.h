/**
 * Layer class
 */
#ifndef LAYER_H
#define LAYER_H

#include <string>
#include <iostream>
#include <vector>
#include <complex>
#include "Utils.h"
#include "quest.h"
#include <Eigen/Eigenvalues>

using namespace std;
typedef complex<double> C;

class Layer {
 private:
    vector<double> angles_array;
    int angl_pos;
    int layer_n_qubits;
    int layer_depth; 
    vector<int> measurement_setting;

 public:

    Layer(int n_qubits, int depth);
    
    void applyLayer(Qureg &ds_qreg, int n_qubits, string &backend);

    void measurement_layer(Qureg &ds_qreg, int n_qubits, string backend);
    
    void simlayer(Qureg &ds_qreg, int n_qubits, float p1, float p2, int &angl_pos);

    void measurement_simlayer(Qureg &ds_qreg, int n_qubits);

    void metrics(Qureg &ds_qreg, int dim);

    vector<int> getMeasurementSetting();
};

#endif