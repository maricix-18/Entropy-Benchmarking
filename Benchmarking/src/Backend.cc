#include "Backend.h"

using namespace std;

void Backend::some_backendfunc()
{
};

void Backend::applyLayer(Qureg &ds_qreg, int &st_qubit, int &fn_qubit, vector<double> &angles_array, int &depth)
{
};

void Backend::measurementLayer(Qureg &ds_qreg, int &qubits, vector<int> &basis)
{
};

double Backend::get_p1_local() {
    return 0;
};

double Backend::get_p2_local() {
    return 0;
};

double Backend::get_p1_global() {
    return 0;
};

double Backend::get_p2_global() {
    return 0;
};