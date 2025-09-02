#ifndef DM_H
#define DM_H

#include "Protocol.h"
#include <fstream>
#include <sys/stat.h>
#include <Eigen/Eigenvalues>
#include "nlohmann/json.hpp"

typedef complex<double> Complex_value;
typedef Eigen::MatrixXcd Eigen_matrix;

using namespace std;
using json = nlohmann::ordered_json;

class DensityMatrix : public Protocol
{
protected:
    vector<double> all_purity;
    vector<double> all_R2d;
 public:
    void somefunc() override;
   
    void metrics() override;

    void saveMetrics() override;

};

#endif