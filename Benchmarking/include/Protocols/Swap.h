#ifndef SWAP_H
#define SWAP_H

#include "Protocol.h"
#include <fstream>
#include <sys/stat.h>
#include <random>
#include <string>
#include <iomanip>
#include <Eigen/Eigenvalues>
#include "nlohmann/json.hpp"

typedef complex<double> Complex_value;
typedef Eigen::MatrixXcd Eigen_matrix;

using namespace std;
using json = nlohmann::ordered_json;

class Swap : public Protocol
{
protected:
    vector<double> all_purity_mean;
    vector<double> all_R2d_mean;
    vector<double> all_purity_std;
    vector<double> all_R2d_std;
    vector<double> pur_samples;
    vector<double> R2d_samples;
    double num_measurements = 320; // M
    int shots = 1000; // k
    int groups = 5;
    int samples = 3; // samples
    double K_factor = 1.0 / (shots * shots);
    int M_subgroup =static_cast<int>(ceil(double(num_measurements / groups)));
    double M_factor = 2.0 / (M_subgroup * (M_subgroup - 1));

    vector<int> key;
    vector<qreal> prob_dist;
    map<string, int> counts;
    vector<double> means;
    
 public:
    void somefunc() override;
   
    void metrics() override;

    void saveMetrics() override;

    void swap_layer();

    double estimate_purity_from_swap_test(map<string, int> &counts);

    int swap_test_outcome(string &outcome);

    int parity_bit(string& bitstring);

    string bitwise_AND(string& str1, string& str2);

    double median(vector<double> &means);

};

#endif