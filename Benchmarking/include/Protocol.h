#ifndef PROTOCOL_H
#define PROTOCOL_H

#include "quest.h"
#include "Backend.h"
#include <iostream>

extern "C"
{
  #include "./mt19937ar.h"

}

using namespace std;

#define PI 3.14159265358979323846

class Protocol
{
 protected:
    string this_prot;
    Backend* backend; // pointer to base class Backend
    Qureg ds_qreg;
    int _qubits;
    int _depth;
    vector<double> angles_array;

 public:

   virtual void somefunc();

   void init(int &qubits, int &depth);

   void setBackend(Backend &_backend);

   void setProtocol(string &ptr);

   void buildCircuit(int &st_qubit, int &fn_qubit);

   virtual void measurement();
   
   virtual void metrics();

   virtual void saveMetrics();

   void angles_generator();
   void destroy();

};

#endif