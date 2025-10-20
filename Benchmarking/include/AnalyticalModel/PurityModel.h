#ifndef PURITYMOD_H
#define PURITYMOD_H

#pragma once
#include "Backend.h"
#include <iostream>

using namespace std;

class PurityModel
{
protected:
    Backend* backend;
    int _qubits;
    int _max_depth;
    double p1;
    double p2;
    vector<double> all_pur;
    vector<double> all_R2d;

public:
    void initialise(Backend &_backend, int &qubits, int &max_depth);
    void purityModel_globalDP();

    void purityModel_globalDP_CS();

    void purityModel_globalDP_localDP();

};
#endif