#include "Backend.h"
#include <iostream>
using namespace std;

void Backend::some_backendfunc()
{
    cout << "Backend function called." << endl;
};

void Backend::applyLayer(Qureg &ds_qreg, int &st_qubit, int &fn_qubit, vector<double> &angles_array, int &depth)
{
    cout << "Applying layer in Backend." << endl;
};

void Backend::measurementLayer(Qureg &ds_qreg, int &qubits, vector<int> &basis)
{
    cout << "Measurement layer in Backend." << endl;
};