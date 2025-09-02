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

    virtual void applyLayer(Qureg &ds_qreg, int &qubits, vector<double> &angles_array);

    virtual void measurementLayer(Qureg &ds_qreg, int &qubits, vector<int> &basis);
    
    //virtual ~Backend() = default;

};

#endif