#ifndef PURITYMOD_H
#define PURITYMOD_H

#pragma once
#include "Backend.h"
#include <Eigen/SVD>
#include <unsupported/Eigen/LevenbergMarquardt>
#include <unsupported/Eigen/AutoDiff>

#include "nlohmann/json.hpp"
using json = nlohmann::ordered_json;
#include <numeric>
#include <iostream>
#include <fstream>

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

    double purity_model_globalDP_CS_circuit_measerr(double &d, double &alpha_1, double &alpha_2, double &beta);

    double purity_model_globalDP_CS_circuit_measerr_part_eval(int &d, double &alpha_2, double &beta);

    vector<double> linspace(int start, int stop, int num);

    struct PurityFitFunctor : Eigen::DenseFunctor<double> {
        PurityModel& model;            // reference to parent
        vector<int>& xdata;
        vector<double>& ydata;

        // Constructor
        PurityFitFunctor(PurityModel& m, vector<int>& x, vector<double>& y)
            : Eigen::DenseFunctor<double>(2, static_cast<int>(x.size())),
              model(m), xdata(x), ydata(y) {}

        int operator()(const Eigen::VectorXd &params, Eigen::VectorXd &residuals) const {
            double alpha_2 = params[0];
            double beta = params[1];
            for (int i = 0; i < values(); ++i) {
                // Call the member function via the parent instance
                double model_val = model.purity_model_globalDP_CS_circuit_measerr_part_eval(xdata[i], alpha_2, beta);
                residuals[i] = ydata[i] - model_val;
            }
            return 0;
        }
    };

    pair<Eigen::VectorXd, Eigen::MatrixXd> curve_fit_eigen(vector<int>& xdata,vector<double>& ydata,Eigen::VectorXd p0,Eigen::VectorXd& lb,Eigen::VectorXd& ub);
    void saveMetrics();

};
#endif