/**
 * Template of create a circuit with specific gate and noise parameters.
 */
#ifndef CIRCUIT_H
#define CIRCUIT_H

#include <string>
#include <iostream>
#include "Layer.h"
#include "quest.h"

using namespace std;

template <class T>
class Circuit {
    T layer;
public:
    Circuit(int n_qubits, int depth) : layer(n_qubits, depth){
    }
  
    void createCircuit(Qureg &ds_qreg, int n_qubits, int depth, string &backend) {
        cout << "Create Circuit function. \n";
        for (int i = 0; i < depth; i++) {
            layer.applyLayer(ds_qreg, n_qubits, backend);
        }
    }

    // measurement
    void measurement(Qureg &ds_qreg, int n_qubits, string backend) {
        //cout << "Measurement function. \n";
        layer.measurement_layer(ds_qreg, n_qubits, backend);
    }

    vector<int> getMeasurementSetting(int qubits) {
        return layer.getMeasurementSetting(qubits);
    }

}; 

#endif