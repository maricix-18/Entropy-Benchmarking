#include "DensityMatrix.h"

void  DensityMatrix::somefunc() {
    backend->some_backendfunc();
};

void  DensityMatrix::metrics()
{
    int dim = int (pow(2, _qubits));
    Eigen_matrix eigen_mat(dim, dim);

    for (int i = 0; i < dim; i++)
    {
        for (int j = 0; j < dim; j++)
        {
            Complex_value amplitude = getDensityQuregAmp(ds_qreg, i, j);
            eigen_mat(i,j) = Complex_value(amplitude.real(), amplitude.imag());
        }
    }

    /** The matrix is first reduced to Schur form using the ComplexSchur class.
    *  The Schur decomposition is then used to compute the eigenvalues and eigenvectors.
    *  The cost of the computation is dominated by the cost of the Schur decomposition, which is O(n^3) where n is the size of the matrix.
    */
    EigenSolver eigen_solv;
    eigen_solv.compute(eigen_mat, false); // compute only eingenvalues, not eigenvectors
    Complex_value sum_entropy = 0;
    for (int e = 0; e < dim; e++)
    {
        Complex_value eigen_val = eigen_solv.eigenvalues()(e);
        double eigen_val_abs = abs(eigen_val);
        // make sure it s computable
        if (eigen_val_abs < 1e-12) continue;
        //calculate ln
        Complex_value log_z = log(eigen_val_abs) + 1i * arg(eigen_val);
        Complex_value log2_z = log_z / log(2.0);
        sum_entropy += (-eigen_val * log2_z);
    }

    double vNd = abs(sum_entropy/double(_qubits));
    double pur = (eigen_mat*eigen_mat).trace().real();
    double R2d = -1 * log2(pur) / _qubits;

    all_purity.push_back(pur);
    all_R2d.push_back(R2d);
    all_VnD.push_back(vNd);

};

void DensityMatrix::saveMetrics()
{
    cout << "Saving DensityMatrix metrics to file." << endl;
    json j;
    string filename;
    #ifdef _WIN32
        filename = "../../Data_test/DensityMatrices_metrics/Q" + to_string(_qubits) + ".json";;
    #else
        filename = "../Data_test/DensityMatrices_metrics/Q" + to_string(_qubits) + ".json";;
    #endif

    // check file or create
    struct stat buffer;
    if (stat(filename.c_str(), &buffer) == 0) {
        ifstream in(filename);
        if (in.is_open()) {
            // read your data here
            cout << "File opened for reading.\n";
        }
    } else {
        ofstream out(filename);
        if (out.is_open()) {
            // create or write initial content here
            cout << "File created.\n";
        }
    }
    for (double VnD : all_VnD) {
        j["all_VnD_diff_n"].push_back(VnD);
    }
    for (double pur : all_purity) {
        j["all_pur_diff_n"].push_back(pur);
    }
    for (double R2d : all_R2d) {
        j["all_R2d_diff_n"].push_back(R2d);
    }
    
    ofstream out(filename);
    if (out.is_open())
        out << setw(4) << j << endl;
};