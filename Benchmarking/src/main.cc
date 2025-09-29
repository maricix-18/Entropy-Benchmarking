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
    int qubits = 3;
    int depth = 15;

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
    cin >> backend_choice;

    if (backend_choice == 1) {
        cout << "Simulator backend chosen." << endl;
        backend_ptr = make_unique<Simulator>();
        backend_ptr->some_backendfunc();
    
    } else {
        cout << "Invalid choice." << endl;
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
        cout << "Invalid choice." << endl;
    }
   
    int st = 0;
    int fn = qubits;
    protocol_ptr->setBackend(*backend_ptr);

    // comment this out if you want multiple depths
    // protocol_ptr->init(qubits, depth);
    // protocol_ptr->buildCircuit(st, fn);
    // protocol_ptr->metrics();
    // protocol_ptr->saveMetrics();
    // protocol_ptr->destroy();

    // for multiple depths
    // otherwise only one last depth is stored

    for (int d =0; d<depth; d++)
    {
        cout << "-Depth " << d << endl;
        protocol_ptr->init(qubits, d);
        protocol_ptr->buildCircuit(st, fn);
        protocol_ptr->metrics();
        protocol_ptr->saveMetrics();
        protocol_ptr->destroy();
    }
   
    return 0;
}