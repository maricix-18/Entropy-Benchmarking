#include <iostream>
#include <string>
#include <memory>
#include "Backend.h"
#include "Simulator.h"
#include "Protocol.h"
#include "DensityMatrix.h"
#include "ClassicalShadows.h"
#include "Swap.h"

using namespace std;

int main(int argc, char* argv[]) {
    int qubits = 5;
    int max_depth = 15;

    int backend_choice;
    int protocol;

    unique_ptr<Backend> backend_ptr;
    unique_ptr<Protocol> protocol_ptr;

    cout << "Hello, Entropy Benchmarking!" << endl;
        
    cout << " Choose a simulation or protocol:" << endl;
    cout << " 1 Density Matrix" << endl;
    cout << " 2 Classical Shadows" << endl;
    cout << " 3 Swap Test" << endl;
    cout << " Input the number of the protocol you want to use: ";
    cin >> protocol;

    cout << " Choose Backend: " << endl;
    cout << " 1 Simulator" << endl;
    cout << " Input the number of the backend you want to use: ";
    cin >> backend_choice;

    if (backend_choice == 1) {
        cout << "Simulator backend chosen." << endl;
        backend_ptr = make_unique<Simulator>();
        backend_ptr->some_backendfunc();
    
    } else {
        cout << "Invalid choice - Default Simulator." << endl;
        backend_ptr = make_unique<Simulator>();
        backend_ptr->some_backendfunc();
    }

    if (protocol == 1) 
    {
        cout << "Density Matrix." << endl;
        protocol_ptr = make_unique<DensityMatrix>();

    }
    else if (protocol == 2)
    {
        cout << "Classical Shadows protocol." << endl; 
        protocol_ptr = make_unique<ClassicalShadows>();
    }
    else if (protocol == 3)
    {
        cout << "Swap Test protocol." << endl;
        protocol_ptr = make_unique<Swap>();
        string prot = "Swap";
        protocol_ptr->setProtocol(prot);
    }
    else 
    {
        cout << "Invalid choice - Default Density Matrix." << endl;
        protocol_ptr = make_unique<DensityMatrix>();
    }
   
    int st = 0;
    int fn = qubits;
    protocol_ptr->initialise(*backend_ptr, qubits, max_depth);

    for (int curr_depth = 4; curr_depth <= max_depth; curr_depth++)
    {
        cout << " - Depth " << curr_depth << endl;
        protocol_ptr->setQureg();
        protocol_ptr->buildCircuit(curr_depth);
        protocol_ptr->metrics();
        protocol_ptr->saveMetrics();
        protocol_ptr->destroy();
    }
   
    return 0;
}