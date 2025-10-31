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

    void purityModel_globalDP_localDP();

    void purityModel_globalDP_CS();

    void purityModel_globalDP();

    double purity_model_globalDP_CS_circuit_measerr(int &d, double &alpha_1, double &alpha_2, double &beta);

    double purity_model_globalDP_CS_circuit_measerr_part_eval(int &d, double &alpha_2, double &beta);

};
#endif