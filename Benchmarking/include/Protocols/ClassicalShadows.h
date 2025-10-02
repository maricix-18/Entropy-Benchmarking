#ifndef CS_H
#define CS_H

#include "Protocol.h"
#include <fstream>
#include <random>
#include <iomanip>
#include <sys/stat.h>
#include <Eigen/Eigenvalues>
#include "nlohmann/json.hpp"

typedef complex<double> Complex_value;
typedef Eigen::MatrixXcd Eigen_matrix;

using namespace std;
using json = nlohmann::ordered_json;

// more complicated data structures for now
typedef map<string, vector<double>> Beta_map;
typedef vector<pair<vector<int>, map<string, int>>> Shadow_map; // vector of <measurement setting, counts>
//typedef pair<vector<int>, map<string, int>> Shadow_list;

class ClassicalShadows : public Protocol
{
protected:
    vector<double> all_purity_mean;
    vector<double> all_R2d_mean;
    vector<double> all_purity_std;
    vector<double> all_R2d_std;
    Beta_map beta_values;
    vector<double> pur_samples;
    vector<double> R2d_samples;
    Shadow_map shadow_map;

    // CS data to set
    // TODO issue with subgroup number, make sure you fix it.
    // to keep clear of errors, make sure that num_meas / groups
    // doesn't leave remainder
    double num_measurements = 320; // M
    int shots = 1000; // k
    int groups = 5;
    int samples = 3; // samples
    double K_factor = 1.0 / (shots * shots);
    int M_subgroup = static_cast<int>(ceil(double(num_measurements / groups)));
    double M_factor = 2.0 / (M_subgroup * (M_subgroup - 1));
    bool verbose = true;

    vector<int> key;
    vector<qreal> prob_dist;
    map<string, int> counts; // map for unique bitstring gathering
    vector<double> means;
  
 public:
    void somefunc() override;
   
    void metrics() override;

    void saveMetrics() override;

    void beta_vals_paulibasis();

    double median(std::vector<double> &vec);

    void gatherShadows();
    
    vector<int> generate_measurement_setting();

    void classicalShadows_protocol();

};

#endif