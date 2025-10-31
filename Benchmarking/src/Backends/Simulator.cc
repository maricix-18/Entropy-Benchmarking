#include "Simulator.h"

void Simulator::some_backendfunc() {
    //cout << "Simulator backend function called." << endl;
};

void Simulator::applyLayer(Qureg &ds_qreg, int &st_qubit, int &fn_qubit, vector<double> &angles_array, int &depth)
{
    // keep trakc of current angle pos of current depth
    int angl_pos = 2 * (fn_qubit - st_qubit) * (depth - 1); // starting pos for the angle array / depth

    for (int q = st_qubit; q < fn_qubit; q++)
    {
        applyRotateX(ds_qreg, q, angles_array[angl_pos]);
        mixDepolarising(ds_qreg, q, p1);
        angl_pos++;
    }

    for (int q = st_qubit; q < fn_qubit; q++)
    {
        applyRotateY(ds_qreg, q, angles_array[angl_pos]);
        mixDepolarising(ds_qreg, q, p1);
        angl_pos++;
    }

    // for each layer add 2x cz layer on nearest neighbour qubits
    for (int q2 = 0; q2 < 2; q2++)
    {
        for (int q = q2 + st_qubit; q < fn_qubit - 1; q+=2)
        {
            applyControlledPauliZ(ds_qreg, q, q+1);
            mixTwoQubitDepolarising(ds_qreg, q, q+1, p2);
        }
    }
};

void Simulator::measurementLayer(Qureg &ds_qreg, int &qubits, vector<int> &basis)
{
    //cout << "Simulator::measurementLayer called with " << qubits << " qubits." << endl;
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