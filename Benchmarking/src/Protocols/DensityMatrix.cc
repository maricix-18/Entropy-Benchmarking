#include "DensityMatrix.h"

void  DensityMatrix::somefunc() { 
    cout << "DensityMatrix protocol function called." << endl;
    backend->some_backendfunc();
};

void  DensityMatrix::metrics()
{
    cout << "DensityMatrix metrics function called for last depth." << endl;
    int dim = int (pow(2, _qubits));
    Eigen_matrix eing_mat(dim, dim);

    for (int i = 0; i < dim; i++)
    {
        for (int j = 0; j < dim; j++)
        {
            Complex_value amplitude = getDensityQuregAmp(ds_qreg, i, j);
            eing_mat(i,j) = Complex_value(amplitude.real(), amplitude.imag());
        }
    }

    double pur = (eing_mat*eing_mat).trace().real();
    double R2d = -1 * log2(pur) / _qubits;

    all_purity.push_back(pur);
    all_R2d.push_back(R2d);

    cout<< "purity: "<< pur <<"\n"<< "R2d: " << R2d <<"\n";

};

void DensityMatrix::saveMetrics()
{
    cout << "Saving DensityMatrix metrics to file." << endl;
    json j;
    string filename = "../Data_test/DensityMatrices_metrics/Q" + to_string(_qubits) + ".json";
    // check file or create
    struct stat buffer;
    if (stat(filename.c_str(), &buffer) == 0) {
        ifstream in(filename);
        if (in.is_open()) {
            // read your data here
            std::cout << "File opened for reading.\n";
        }
    } else {
        ofstream out(filename);
        if (out.is_open()) {
            // create or write initial content here
            std::cout << "File created.\n";
        }
    }

    for (double pur : all_purity) {
        j["all_pur_diff_n"].push_back(pur);
    }
    for (double R2d : all_R2d) {
        j["all_R2d_diff_n"].push_back(R2d);
    }
    
    ofstream out(filename);
    if (out.is_open())
        out << std::setw(4) << j << std::endl;
};
