#ifndef CS_H
#define CS_H

#include "Protocol.h"
#include <fstream>
#include <random>
#include <Eigen/Eigenvalues>
#include "nlohmann/json.hpp"

typedef complex<double> Complex_value;
typedef Eigen::MatrixXcd Eigen_matrix;

using namespace std;
using json = nlohmann::ordered_json;

// more complicated data structures for now
typedef map<string, vector<double>> Beta_map; // beta table for pauli measurements
typedef vector<pair<vector<int>, map<string, int>>> Shadow_map; // vector of <measurement setting, counts>

class ClassicalShadows : public Protocol
{
protected:
    vector<double> all_purity_mean;
    vector<double> all_R2d_mean;
    vector<double> all_purity_std;
    vector<double> all_R2d_std;
    vector<double> pur_samples;
    vector<double> R2d_samples;
    Shadow_map shadow_map;
    Beta_map beta_values;
    vector<int> key;
    map<string, int> counts; // map for unique bitstring gathering
    vector<double> means;
    double num_measurements = 320; // M
    int shots = 1000; // k
    int groups = 5;
    int samples = 3; // samples
    double K_factor = 1.0 / (shots * shots);

    // TODO issue with subgroup number, make sure you fix it.
    // to keep clear of errors, make sure that num_meas / groups
    // doesn't leave remainder
    int M_subgroup = static_cast<int>(ceil(double(num_measurements / groups)));
    double M_factor = 2.0 / (M_subgroup * (M_subgroup - 1));
  
 public:
    void somefunc() override;
   
    void metrics() override;

    void saveMetrics() override;

    void beta_vals_paulibasis();

    double median(vector<double> &vec);

    void gatherShadows();
    
    vector<int> generate_measurement_setting();

    void classicalShadows_protocol();

    int binarySearchCDF(vector<double>& cdf, double value);

};

#endif