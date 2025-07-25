/**
 * Layer class
 */
#include "Layer.h"

Layer::Layer(int n_qubits, int depth)
{
    layer_n_qubits = n_qubits;
    layer_depth = depth;
    cout << "Constructor Layer. \n";
    angl_pos = 0;
    // generate the angles for the circuit
    angles_generator(n_qubits, depth, angles_array);
    cout<< "Size of angles array: "<< angles_array.size() << "\n";
}
    
void Layer::applyLayer(Qureg &ds_qreg, int n_qubits, string &backend) {
    
    // get backend info
    if (backend == "Simulator")
    {
        // noise params
        // Depolarising noise 1 q gate
        float p1 = 0.008;
        //p1 = (3 * p1 / 4.0); // scale for quest function

        // Depolarising noise 2q gate
        float p2 = 0.054;
        //p2 = (15 * p2 / 16.0); // scale for quest function
        //cout << "Apply sim layer. \n";
        simlayer(ds_qreg, n_qubits, p1, p2, angl_pos);
        metrics(ds_qreg,n_qubits);
    }
    
};

void Layer::measurement_layer(Qureg &ds_qreg, int n_qubits, string backend)
{
    measurement_setting.clear();
    
    if (backend == "Simulator")
    {
        measurement_simlayer(ds_qreg, n_qubits);
    }
};
    
void Layer::simlayer(Qureg &ds_qreg, int n_qubits, float p1, float p2, int &angl_pos)
{
    //cout<< "Appyling sim layer. \n";
    // rx + noise, ry + noise, cz + noise
    for (int j = 0; j < n_qubits; j++)
    {
        //cout << "Angle pos: " << angl_pos <<"\n";
        //cout<<"rx gate: Angle = "<<angles_array[angl_pos]<<", Applied to q[" << j <<"\n";
        applyRotateX(ds_qreg, j, angles_array[angl_pos]);
        mixDepolarising(ds_qreg, j, p1);
        angl_pos++;
    }

    // for each layer add y rotation on all qubits
    for (int j = 0; j < n_qubits; j++)
    {
        //cout << "Angle pos: " << angl_pos <<"\n";
        //cout<<"ry gate: Angle = "<<angles_array[angl_pos]<<", Applied to q[" << j <<"\n";
        applyRotateY(ds_qreg, j, angles_array[angl_pos]);
        mixDepolarising(ds_qreg, j, p1);
        angl_pos++;
    }

    // for each layer add 2x cz layer on nearest neighbour qubits
    for (int g = 0; g < 2; g++)
    {
        for (int j = g; j < n_qubits-1; j+=2)
        {
           // cout<<"cz gate applied to control " << j <<" target: "<<j+1 <<"\n";
            applyControlledPauliZ(ds_qreg, j, j+1);
            mixTwoQubitDepolarising(ds_qreg, j, j+1,p2);
        }
    }
};

void Layer::measurement_simlayer(Qureg &ds_qreg, int qubits)
{
    // generate list of random measurments 
    // 0 - X, 1 - Y, 2 - Z
    generate_measurement_setting(measurement_setting, qubits);
    for (int j = 0; j < qubits; j++)
    {
        if (measurement_setting[j] == 0)
        {
            // X basis measurement -- apply H
            applyHadamard(ds_qreg, j);
        }
        else if (measurement_setting[j] == 1)
        {
            // Y basis measurement 
            // S^dagger = RZ(-pi/2)
            applyRotateZ(ds_qreg, j, -PI/2);
            // H
            applyHadamard(ds_qreg, j);
        }
        // Z basis measurement -- identity
    }
};

void Layer::metrics(Qureg &ds_qreg, int n_qubits)
{
    int dim = pow(2, n_qubits);
    Eigen::MatrixXcd eing_mat(dim, dim);
    for (int i = 0; i < dim; i++)
    {
        for (int j = 0; j < dim; j++)
        {
            complex<double> amplitude = getDensityQuregAmp(ds_qreg, i, j);
            eing_mat(i,j) = C(amplitude.real(), amplitude.imag());
        }
    }

    double pur = (eing_mat*eing_mat).trace().real();
    double R2d = -1 * log2(pur) / n_qubits;

    cout<< "purity: "<< pur <<"\n"<< "R2d: " << R2d <<"\n";
};

vector<int> Layer::getMeasurementSetting()
{
    cout << "Get Measurement Setting function: ";
    for (int i = 0; i < measurement_setting.size(); i++)
    {
        cout << measurement_setting[i];
    }
    cout << "\n";
    return measurement_setting;
};

 