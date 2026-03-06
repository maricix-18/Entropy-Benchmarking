#include <iostream>
#include "Backend.h"
#include "Simulator.h"
#include "Protocol.h"
#include "DensityMatrix.h"
#include "ClassicalShadows.h"
#include "Swap.h"
#include "PurityModel.h"

using namespace std;

int main(int argc, char* argv[]) {
    int backend_choice;
    int protocol;

    unique_ptr<Backend> backend_ptr;
    unique_ptr<Protocol> protocol_ptr;

    cout << "Hello, Entropy Benchmarking!" << endl;
        
    cout << " Choose a simulation or protocol:" << endl;
    cout << " 1 Density Matrix" << endl;
    cout << " 2 Classical Shadows" << endl;
    cout << " 3 Swap Test" << endl;
    cout << " 4 Purity Model" << endl;
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
    else if (protocol == 4)
    {
        // experiment for multiple qubit size
        cout << "Purity Model." << endl;
        PurityModel purity_model;

        int pur_model;

        cout << " Choose a purity model:" << endl;
        cout << " 1 Purity model based on global depolarising noise model." << endl;
        cout << " 2 Purity model based on global depolarising noise model function of local depolarising probabilities." << endl;
        cout << " 3 Purity model based on global depolarising noise model + Classical Shadows." << endl;
        cout << " 4 Purity model based on global depolarising noise no fitting model + alpha_1 = p1, alpha_2 = p2" << endl;
        cout << " Input the number of the protocol you want to use: ";
        cin >> pur_model;
        if (pur_model == 1)
        {
            int max_depth = 5;
            for (int qub = 8; qub <= 17; qub++)
            {
                cout << "\n=== Running experiment for Q" << qub << " ===" << endl;
                purity_model.initialise(*backend_ptr, qub, max_depth);
                purity_model.purityModel_globalDP();
                purity_model.saveMetrics();

                // Save all metrics for this qubit size after all depths are processed
                cout << "Completed qubit: " << qub << endl;
            }
            // purity_model.purityModel_globalDP();
            // purity_model.saveMetrics();
        }
        else if (pur_model == 2)
        {
            int max_depth = 15;
            for (int qub = 2; qub <= 7; qub++)
            {
                cout << "\n=== Running experiment for Q" << qub << " ===" << endl;
                purity_model.initialise(*backend_ptr, qub, max_depth);
                purity_model.purityModel_globalDP_localDP();
                purity_model.saveMetrics();

                // Save all metrics for this qubit size after all depths are processed
                cout << "Completed qubit: " << qub << endl;
            }
            // purity_model.purityModel_globalDP_localDP();
            // purity_model.saveMetrics();
        }
        else if (pur_model == 3)
        {

            purity_model.purityModel_globalDP_CS();
            purity_model.saveMetrics();
        }
        else if (pur_model == 4)
        {

            int max_depth = 15;
            for (int qub = 2; qub <= 15; qub++)
            {
                cout << "\n=== Running experiment for Q" << qub << " ===" << endl;
                purity_model.initialise(*backend_ptr, qub, max_depth);
                purity_model.purityModel_no_fitting();
                purity_model.saveMetrics();

                // Save all metrics for this qubit size after all depths are processed
                cout << "Completed qubit: " << qub << endl;
            }
            // purity_model.purityModel_no_fitting();
            // purity_model.saveMetrics();
        }
        else
        {
            cout << "Invalid choice - Default - 1 Purity model based on global depolarising noise model." << endl;
        }

        cout << "All done." << endl;
        return 0;
    }
    else
    {
        cout << "Invalid choice - Default Density Matrix." << endl;
        protocol_ptr = make_unique<DensityMatrix>();
    }

    // Experiment values
    // int max_qubits = 8;
    // int max_depth = 5;
    //
    // // experiment with multiple qubit sizes
    // for (int qub = 2; qub <= max_qubits; qub++)
    // {
    //     cout << "\n=== Running experiment for Q" << qub << " ===" << endl;
    //     protocol_ptr->initialise(*backend_ptr, qub, max_depth);
    //
    //     for (int curr_depth = 0; curr_depth <= max_depth; curr_depth++)
    //     {
    //         cout << " - Depth " << curr_depth << endl;
    //         protocol_ptr->setQureg();
    //         protocol_ptr->buildCircuit(curr_depth);
    //         protocol_ptr->metrics();
    //         protocol_ptr->destroy();
    //     }
    //
    //     // Save all metrics for this qubit size after all depths are processed
    //     protocol_ptr->saveMetrics();
    //     cout << "Completed Experiment." << qub << endl;
    // }


    return 0;
}