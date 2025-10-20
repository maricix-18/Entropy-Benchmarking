#ifndef DM_H
#define DM_H

#include "Protocol.h"

typedef complex<double> Complex_value;
typedef Eigen::MatrixXcd Eigen_matrix;
typedef Eigen::ComplexEigenSolver<Eigen::MatrixXcd> EigenSolver;

using namespace std;
using json = nlohmann::ordered_json;

class DensityMatrix : public Protocol
{
protected:
    vector<double> all_VnD;
    vector<double> all_purity;
    vector<double> all_R2d;
 public:
    void somefunc() override;
   
    void metrics() override;

    void saveMetrics() override;

};

#endif