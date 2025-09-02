#include "Simulator.h"

void Simulator::some_backendfunc() {
    cout << "Simulator backend function called." << endl;
};

void Simulator::applyLayer(Qureg &ds_qreg, int &qubits, vector<double> &angles_array)
{
    cout << "Simulator::applyLayer called with " << qubits << " qubits." << endl;
    int angl_pos = 0;
    //cout<< "Appyling sim layer. \n";
    // rx + noise, ry + noise, cz + noise
    for (int q = 0; q < qubits; q++)
    {
        //cout << "Angle pos: " << angl_pos <<"\n";
        //cout<<"rx gate: Angle = "<<angles_array[angl_pos]<<", Applied to q[" << j <<"\n";
        applyRotateX(ds_qreg, q, angles_array[angl_pos]);
        mixDepolarising(ds_qreg, q, p1);
        angl_pos++;
    }

    // for each layer add y rotation on all qubits
    for (int q = 0; q < qubits; q++)
    {
        //cout << "Angle pos: " << angl_pos <<"\n";
        //cout<<"ry gate: Angle = "<<angles_array[angl_pos]<<", Applied to q[" << j <<"\n";
        applyRotateY(ds_qreg, q, angles_array[angl_pos]);
        mixDepolarising(ds_qreg, q, p1);
        angl_pos++;
    }

    // for each layer add 2x cz layer on nearest neighbour qubits
    for (int q2 = 0; q2 < 2; q2++)
    {
        for (int q = q2; q < qubits-1; q+=2)
        {
           // cout<<"cz gate applied to control " << j <<" target: "<<j+1 <<"\n";
            applyControlledPauliZ(ds_qreg, q, q+1);
            mixTwoQubitDepolarising(ds_qreg, q, q+1,p2);
        }
    }
};

void Simulator::measurementLayer(Qureg &ds_qreg, int &qubits, vector<int> &basis)
{
    cout << "Simulator::measurementLayer called with " << qubits << " qubits." << endl;
    for (int j = 0; j < qubits; j++)
    {
        //cout<< "Applying meas " << measurement_setting[j]<< " on qubit " << j <<endl;
        if (basis[j] == 0) 
        {
            // X basis measurement -- apply H
            applyHadamard(ds_qreg, j);
            //cout<< "applied hadamard"<<endl;
        }
        else if (basis[j] == 1)
        {
            // Y basis measurement 
            // S^dagger = RZ(-pi/2)
            applyRotateZ(ds_qreg, j, -PI/2);
            // H
            applyHadamard(ds_qreg, j);
        }
        // Z basis measurement -- identity
    }
    //cout<<"measuremnt for sim layer applied"<<endl;
};