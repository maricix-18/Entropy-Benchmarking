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
double PurityModel::purity_model_globalDP_CS_circuit_measerr_part_eval(int &d, double &alpha_2, double &beta) {
    double c = p1/p2;
    double alpha_1 = alpha_2 * c;
    return  0;
};

double PurityModel::purity_model_globalDP_CS_circuit_measerr(int &d, double &alpha_1, double &alpha_2, double &beta) {
    return (1 - pow(2,(-_qubits)))*(exp(-2*(alpha_1*_qubits*(2*d) + alpha_2*(_qubits-1)*d + beta*_qubits)) - 1) + 1;
};

void PurityModel::purityModel_globalDP_CS()
{
    cout << "purityModel_globalDP_CS" << endl;
    double alpha_2 = 0;
    double beta = 0;

    for (int d = 0; d <= _max_depth; d++)
    {
        // for each depth
        double part_eval = purity_model_globalDP_CS_circuit_measerr_part_eval(d, alpha_2, beta);

        // fit curve based on data
        // popt_classim, _ = curve_fit(part_eval, d, short_metrics_classim['all_pur_mean_diff_n'], bounds=(0,1))
        // alpha_1_optim_classim = popt_classim[0] * c
        // alpha_2_optim_classim = popt_classim[0]
        // beta_optim_classim = popt_classim[1]

        // pur = purity_fit(alpha_1_optim_classim, alpha_2_optim_classim, beta_optim_classim, d);

        //double pur = (1 - pow(2,(-_qubits)))*(exp(-2*(alpha_1*_qubits*(2*d) + alpha_2*(_qubits-1)*d+ beta*_qubits)) - 1) + 1;
       // all_pur.push_back(pur);
    }
};

void PurityModel::purityModel_globalDP()
{
    // ?? What is this one?
    cout << "purityModel_globalDP"<< endl;
    double alpha_1 = log(1/(1-p1));
    double alpha_2 = log(1/(1-p2));
    for (int d = 0; d <= _max_depth; d++)
    {
        double pur =  (1 - pow(2,(-_qubits)))*(exp(-2*(2*alpha_1*_qubits + alpha_2 * (_qubits - 1))* d) - 1) + 1;
        all_pur.push_back(pur);
    }
};

