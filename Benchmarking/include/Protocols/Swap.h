#ifndef SWAP_H
#define SWAP_H

#include "Protocol.h"

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
    int num_measurements = 307200; // number of measurements
    int groups = 3;
    int shots =  static_cast<int>(ceil(double(num_measurements / groups))); //K
    int samples = 3; // samples
    map<string, int> counts;
    vector<double> means;

 public:
    void somefunc() override;
   
    void metrics() override;

    void saveMetrics() override;

    void swapLayer();

    double estimatePurityFromSwapTest(map<string, int> &counts);

    int swapTestOutcome(string &outcome);

    int parityBit(string& bitstring);

    string bitwiseAND(string& str1, string& str2);

    double median(vector<double> &means);

    int binarySearchCDF(vector<double>& cdf, double value);

};

#endif