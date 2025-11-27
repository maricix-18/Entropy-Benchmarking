#ifndef SIM_H
#define SIM_H

#pragma once
#include "Backend.h"

#define PI 3.14159265358979323846

class Simulator : public Backend
{
 protected:
    double p1_local = 0.008;
    double p2_local = 0.054;
    double p1 = (3*p1_local)/4.0; // depolarising noise 1 qubit gate
    double p2 = (15*p2_local)/16.0; // depolarising noise 2 qubits gate
    double p1_global = 0.08;
    double p2_global = 0.08;
 public:
    
    void some_backendfunc() override;

    void applyLayer(Qureg &ds_qreg, int &st_qubit, int &fn_qubit, vector<double> &angles_array, int &depth) override;

    void measurementLayer(Qureg &ds_qreg, int &qubits, vector<int> &basis) override;

    double get_p1_local() override;

    double get_p2_local() override;

    double get_p1_global() override;

    double get_p2_global() override;
};

#endif