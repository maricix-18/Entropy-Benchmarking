#ifndef PROTOCOL_H
#define PROTOCOL_H

#include "quest.h"
#include "Backend.h"
#include <sys/stat.h>
#include <Eigen/Eigenvalues>
#include "nlohmann/json.hpp"
#include <random>
#include <string>
#include <fstream>
extern "C"
{
#include "./mt19937ar.h"

}
using namespace std;

#define PI 3.14159265358979323846

class Protocol
{
 protected:
    string this_prot; // for swap register
    Backend* backend; // pointer to base class Backend
    Qureg ds_qreg;
    int _qubits;
    int _depth; // current depth
    int _max_depth; // total number of depths for experiments
    vector<double> angles_array;

 public:

   virtual void somefunc();

   void initialise(Backend &_backend, int &qubits, int &max_depth);

   void setBackend(Backend &_backend);

   void setProtocol(string &ptr);

   void setQureg();

   void buildCircuit(int &curr_depth);

   virtual void measurement();
   
   virtual void metrics();

   virtual void saveMetrics();

   void angles_generator();

   void destroy();

};

#endif