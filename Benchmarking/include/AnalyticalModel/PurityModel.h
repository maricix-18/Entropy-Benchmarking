#ifndef PURITYMOD_H
#define PURITYMOD_H

#pragma once
#include "Backend.h"
#include <Eigen/SVD>
#include <numeric>
#include <iostream>
#include <fstream>
#include <unsupported/Eigen/LevenbergMarquardt>
#include <unsupported/Eigen/AutoDiff>
#include "nlohmann/json.hpp"
using json = nlohmann::ordered_json;
using namespace std;

class PurityModel
{
protected:
    Backend* backend;
    int _qubits;
    int _max_depth;
    double p1_local;
    double p2_local;
    vector<double> all_pur;
    vector<double> all_R2d;
    vector<double> depth_tab_more_points;
    vector<double> depth_tab;

public:
    void initialise(Backend &_backend, int &qubits, int &max_depth);

    void purityModel_globalDP();

    double purityModel_globalDP_value(double &depth, double &alpha_1, double &alpha_2);

    double purityModel_globalDP_R2d_model_part_eval(double &depth, double &alpha_2);

    void purityModel_globalDP_localDP();

    double purityModel_globalDP_localDP_value(double &depth, double &alpha_1, double &alpha_2);

    double purityModel_globalDP_localDP_R2d_model_part_eval(double &depth, double &p_1, double &p_2);

    void purityModel_globalDP_CS();

    double purity_model_globalDP_CS_circuit_measerr(double &d, double &alpha_1, double &alpha_2, double &beta);

    double purity_model_globalDP_CS_circuit_measerr_part_eval(double &d, double &alpha_2, double &beta);

    void depth_tab_more_points_populate();

    void depth_tab_populate();

    vector<double> linspace(int start, int stop, int num);

    struct PurityFitFunctor : Eigen::DenseFunctor<double>
    {
        PurityModel &model;   // pointer to this class functions
        string &model_name;
        vector<double>& xdata;
        vector<double>& ydata;

        // Constructor - keep name to call the right function
        PurityFitFunctor(PurityModel& m, string &name, vector<double>& x, vector<double>& y, int &params_to_fit)
            : Eigen::DenseFunctor<double>(params_to_fit, static_cast<int>(x.size())),
              model(m), model_name(name), xdata(x), ydata(y) {}

        int operator()(const Eigen::VectorXd &params, Eigen::VectorXd &residuals) const
        {
            if (model_name == "purity_model_globalDP") {
                double alpha_2 = params[0];
                for (int i = 0; i < values(); ++i) {
                    double model_val = model.purityModel_globalDP_R2d_model_part_eval(xdata[i], alpha_2);
                    residuals[i] = ydata[i] - model_val;
                }
            }
            else if (model_name == "purity_model_globalDP_CS") {
                double alpha_2 = params[0];
                double beta = params[1];
                for (int i = 0; i < values(); ++i) {
                    double model_val = model.purity_model_globalDP_CS_circuit_measerr_part_eval(xdata[i], alpha_2, beta);
                    residuals[i] = ydata[i] - model_val;
                }
            }
            else if (model_name == "purity_model_localDP") {
                double p_1 = params[0];
                double p_2 = params[1];
                for (int i = 0; i < values(); ++i) {
                    double model_val = model.purityModel_globalDP_localDP_R2d_model_part_eval(xdata[i], p_1, p_2);
                    residuals[i] = ydata[i] - model_val;
                }
            }

             return 0;
        }
    };

    pair<Eigen::VectorXd, Eigen::MatrixXd> curve_fit_eigen(string &name, vector<double>& xdata,vector<double>& ydata,Eigen::VectorXd p0,Eigen::VectorXd& lb,Eigen::VectorXd& ub, int &params_to_fit);

    void saveMetrics();

};
#endif