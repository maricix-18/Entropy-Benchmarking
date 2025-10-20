#include "PurityModel.h"

void PurityModel::initialise(Backend &_backend, int &qubits, int &max_depth) {
    // set backend
    backend = &_backend;

    // init values
    _qubits = qubits;
    _max_depth = max_depth;

    p1 = backend->get_p1();
    p2 = backend->get_p2();
};

void PurityModel::purityModel_globalDP()
{
    // ?? What is thsi one?
    cout << "purityModel_globalDP"<< endl;
    double alpha_1 = log(1/(1-p1));
    double alpha_2 = log(1/(1-p2));
    for (int d = 0; d <= _max_depth; d++)
    {
        double pur =  (1 - pow(2,(-_qubits)))*(exp(-2*(2*alpha_1*_qubits + alpha_2 * (_qubits - 1))* d) - 1) + 1;
        all_pur.push_back(pur);
    }
};

void PurityModel::purityModel_globalDP_CS()
{
    cout << "purityModel_globalDP_CS" << endl;
    double alpha_1 = log(1/(1-p1));
    double alpha_2 = log(1/(1-p2));
    double beta = 0;
    for (int d = 0; d <= _max_depth; d++)
    {
        double pur = (1 - pow(2,(-_qubits)))*(exp(-2*(alpha_1*_qubits*(2*d) + alpha_2*(_qubits-1)*d+ beta*_qubits)) - 1) + 1;
        all_pur.push_back(pur);
    }
};

void PurityModel::purityModel_globalDP_localDP()
{
    cout << "purityModel_globalDP of local depolarising probabilities" << endl;
    double alpha_1 = log(1/(1-p1));
    double alpha_2 = log(1/(1-p2));
    for (int d = 0; d <= _max_depth; d++)
    {
        double pur =  (1 - pow(2,(-_qubits)))*(exp(-2*(2*alpha_1*_qubits + alpha_2 * (_qubits - 1))* d) - 1) + 1;
        all_pur.push_back(pur);
    }
};