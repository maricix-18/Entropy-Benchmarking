#ifndef BACKEND_H
#define BACKEND_H

#pragma once
#include <vector>
#include <iostream>
#include "quest.h"

using namespace std;

class Backend
{

 public:

    virtual void some_backendfunc();

    virtual void applyLayer(Qureg &ds_qreg, int &st_qubit, int &fn_qubit, vector<double> &angles_array, int &depth);

    virtual void measurementLayer(Qureg &ds_qreg, int &qubits, vector<int> &basis);

};

#endif