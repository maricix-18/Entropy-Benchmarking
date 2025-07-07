#include <iostream>
#include <string>
#include "Circuit.h"
#include "quest.h"

using namespace std;

int main(int argc, char* argv[]) {
  int protocol; 

  cout << "Hello, Entropy Benchmarking!" << endl;
    
  cout << " Choose a simulation or protocol:" << endl;
  cout << " 1 Density Matrix" << endl;
  cout << " 2 Classical Shadows" << endl;
  cout << " 3 Swap Test" << endl;
  cout << " Input the number of the protocol you want to use: ";
  cin >> protocol;

  cout << "Create Q Environment\n";
  if (isQuESTEnvInit() == 0)
    initQuESTEnv();
  cout << "Create Q Register\n";
  Qureg ds_qreg = createDensityQureg(4);
  string backend = "Simulator";
  int qubits = 2;
  int depth = 15;

  Circuit<Layer> mydensitymatrix(qubits, depth);

  mydensitymatrix.createCircuit(ds_qreg, qubits, depth, backend);

  switch(protocol) {
    case 1:
      cout << "Density Matrix ." << endl;
      
      break;
    case 2:
      cout << "Classical Shadows protocol." << endl;
      break;
    case 3    :
      cout << "Swap Test protocol." << endl;
  }

  cout << "Destroy Q Register\n";
  destroyQureg(ds_qreg);
  return 0;
}