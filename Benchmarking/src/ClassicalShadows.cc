
/**
 * Classical Shadows Protocol class
 */
#include "ClassicalShadows.h"

void classicalshadows_protocol(int qubits, int depth, string backend)
{
    cout << "Create Q Environment\n";
    if (isQuESTEnvInit() == 0)
      initQuESTEnv();
      
    cout << "Create Q Register\n";
    Qureg ds_qreg = createDensityQureg(qubits);
    Circuit<Layer> mydensitymatrix(qubits, depth);

    cout << "Create Circuit\n";
    mydensitymatrix.createCircuit(ds_qreg, qubits, depth, backend);

    int num_measurements = 3; // M
    int shots = 10; // k
    int groups = 2;
    int samples = 3;

    
    for (int i = 0; i < samples; i++)
    {
        // list of measurements outcome for each shot
        map<vector<int>, vector<vector<int>>> measurement_outcome;

        for (int j = 0; j < num_measurements; j++)
        {
            // use clone to apply measurements
             Qureg clone = createCloneQureg(ds_qreg);

            // apply measurement layer for Pauli shadows
            cout << "Apply Measurement layer\n";
            mydensitymatrix.measurement(clone, qubits, backend);

            // get measurement setting
            vector<int> key = mydensitymatrix.getMeasurementSetting();

            for (int k = 0; k < shots; k++)
            {
                // apply measurement
                cout << "Collapse and get measurement\n";
                vector<int> outcome_bitstring;
                for (int q = 0; q < qubits; q++)
                {
                    int outcome = applyQubitMeasurement(clone, q);
                    cout << "Measurement outcome for qubit " << q << ": " << outcome << "\n";
                    outcome_bitstring.push_back(outcome);
                }
              measurement_outcome[key].push_back(outcome_bitstring);
            }

            cout << "Destroy Clone Q Register\n";
            destroyQureg(clone);
        }
        /*
        * NOTE: for each measurement setting each shot has the same outcome.
        */
        // print measurement outcomes
        for (const auto& entry : measurement_outcome) {
            cout << "Measurement Setting: ";
            for (int val : entry.first) {
                cout << val << " ";
            }
            cout << "\nOutcomes:\n";
            for (const auto& outcome : entry.second) {
                for (int bit : outcome) {
                    cout << bit << " ";
                }
                cout << "\n";
            }
        }

      cout << "Apply Classical Shadows Protocol\n";
      mydensitymatrix.classicalshadows_protocol();

    }

    cout << "Destroy Q Register\n";
    destroyQureg(ds_qreg);

}