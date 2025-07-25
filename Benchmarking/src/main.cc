#include <iostream>
#include <string>
#include "DensityMatrix.h"
#include "ClassicalShadows.h"

using namespace std;

int main(int argc, char* argv[]) {
  int protocol;
  int qubits = 5;
  int depth = 4;

  string backend;
  int backend_choice;
  
  cout << "Hello, Entropy Benchmarking!" << endl;
    
  cout << " Choose a simulation or protocol:" << endl;
  cout << " 1 Density Matrix" << endl;
  cout << " 2 Classical Shadows" << endl;
  cout << " 3 Swap Test" << endl;
  cout << " Input the number of the protocol you want to use: ";
  cin >> protocol;

  cout << " Choose Backend: " << endl;
  cout << " 1 Simulator" << endl; 
  cin >> backend_choice;

  if (backend_choice == 1) {
    backend = "Simulator";
  } else {
    cout << "Invalid choice, defaulting to Simulator." << endl;
    backend = "Simulator";
  }

  switch(protocol) {
    case 1:
      cout << "Density Matrix ." << endl;
      densitymatrix_protocol(qubits, depth, backend);
      break;
    case 2:
      cout << "Classical Shadows protocol." << endl;
      classicalshadows_protocol(qubits, depth, backend);
      break;
    case 3    :
      cout << "Swap Test protocol." << endl;
      cout<< "This protocol is not implemented yet." << endl;
  }
  return 0;
}