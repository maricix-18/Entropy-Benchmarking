#include <iostream>
#include <string>
#include <memory>
#include "Backend.h"
#include "Simulator.h"
#include "Protocol.h"
#include "DensityMatrix.h"
#include "ClassicalShadows.h"

using namespace std;

int main(int argc, char* argv[]) {
    int qubits = 2;
    int depth = 3;

    int backend_choice;
    int protocol;

    //Backend *backend_ptr = nullptr;
    unique_ptr<Backend> backend_ptr;
    //Protocol *protocol_ptr = nullptr;
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
        //Simulator sim;
        backend_ptr = make_unique<Simulator>();
        backend_ptr->some_backendfunc();
    
    } else {
        cout << "Invalid choice." << endl;
    }

    if (protocol == 1) 
    {
        cout << "Density Matrix." << endl;
        //DensityMatrix dm_protocol;
        protocol_ptr = make_unique<DensityMatrix>();
        //protocol_ptr->somefunc();

    }
    else if (protocol == 2)
    {
        cout << "Classical Shadows protocol." << endl; 
        protocol_ptr = make_unique<ClassicalShadows>();
    }
    else if (protocol == 3)
    {
        cout << "Swap Test protocol." << endl;
        cout<< "This protocol is not implemented yet." << endl;
    }
    else 
    {
        cout << "Invalid choice." << endl;
    }
   
    protocol_ptr->setBackend(*backend_ptr);
    protocol_ptr->init(qubits, depth);
    protocol_ptr->buildCircuit();
    protocol_ptr->metrics();
    protocol_ptr->saveMetrics();
    protocol_ptr->destroy();
    // for multiple depths
    // otherwise only one last depth is stored
    // for (int d =0; d<depth; d++)
    // {
    //     cout << "-Depth " << d << endl;
    //     protocol_ptr->init(qubits, d);
    //     protocol_ptr->buildCircuit();
    //     protocol_ptr->metrics();
    //     protocol_ptr->saveMetrics();
    //     protocol_ptr->destroy();
    // }
   
    return 0;
}