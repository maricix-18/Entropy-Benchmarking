#ifndef SIM_H
#define SIM_H

#pragma once
#include "Backend.h"

#define PI 3.14159265358979323846

class Simulator : public Backend
{
 protected:
    double p1 = 0.008; // depolarising noise 1 qubit gate
    double p2 = 0.054; // depolarising noise 2 qubits gate

 public:
    
    void some_backendfunc() override;

    void applyLayer(Qureg &ds_qreg, int &st_qubit, int &fn_qubit, vector<double> &angles_array, int &depth) override;

    void measurementLayer(Qureg &ds_qreg, int &qubits, vector<int> &basis) override;

};

#endif