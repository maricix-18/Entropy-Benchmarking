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

    // 2. fidelity of matrix
    fidelityMetric();

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

bool readNextMatrix(std::ifstream& file, int& depthOut, vector<std::vector<std::complex<double>>>& matrixOut) {
    std::string line;
    std::regex complexRegex(R"(\((-?[0-9eE.+]+),(-?[0-9eE.+]+)\))");

    // Skip lines until we find the next "- Depth"
    while (std::getline(file, line)) {
        if (line.rfind("- Depth", 0) == 0) {
            std::istringstream iss(line);
            std::string dash, depthLabel;
            iss >> dash >> depthLabel >> depthOut;

            matrixOut.clear();

            // Read the next 8 lines (8x8 matrix)
            for (int i = 0; i < 8; ++i) {
                if (!std::getline(file, line)) {
                    std::cerr << "Unexpected end of file while reading matrix at depth " << depthOut << "\n";
                    return false;
                }

                std::vector<std::complex<double>> row;
                std::sregex_iterator it(line.begin(), line.end(), complexRegex);
                std::sregex_iterator end;

                for (; it != end; ++it) {
                    double real = std::stod((*it)[1].str());
                    double imag = std::stod((*it)[2].str());
                    row.emplace_back(real, imag);
                }

                matrixOut.push_back(row);
            }

            return true; // Successfully read a matrix
        }
    }

    return false; // No more matrices
};


void mulMat(const vector<vector<complex<double>>>& m1,
            const vector<vector<complex<double>>>& m2,
            vector<vector<complex<double>>>& res)
{

    int r1 = m1.size();
    int c1 = m1[0].size();
    int r2 = m2.size();
    int c2 = m2[0].size();

    if (c1 != r2) {
        cerr << "Invalid input: matrices cannot be multiplied (m1 cols != m2 rows)" << endl;
        exit(EXIT_FAILURE);
    }

    res.assign(r1, vector<complex<double>>(c2, {0.0, 0.0}));

    for (int i = 0; i < r1; ++i) {
        for (int j = 0; j < c2; ++j) {
            for (int k = 0; k < c1; ++k) {
                res[i][j] += m1[i][k] * m2[k][j];
            }
        }
    }
};

void DensityMatrix::fidelityMetric() {

    // Using the formula of fidelity F(rho, sigma) = F(rho*sigma)= (Sum [sqrt(eing_val)])^2
    int dim = int (pow(2, _qubits));
    // get the two matrices rho and sigma
    string filename1;
    string filename2;
    #ifdef _WIN32
        filename1 = "../../Data_test/DensityMatrices_metrics/MatrixComparison/Q3_D0_D5_quest.log";
        filename2 = "../../Data_test/DensityMatrices_metrics/MatrixComparison/Q3_D0_D5_qiskit.log";
    #else
        filename1 = "../Data_test/DensityMatrices_metrics/MatrixComparison/Q3_D0_D5_quest.log";
        filename2 = "../Data_test/DensityMatrices_metrics/MatrixComparison/Q3_D0_D5_qiskit.log";
    #endif

    ifstream file1(filename1);
    ifstream file2(filename2);

    if (!file1 || !file2) {
        std::cerr << "Error opening one of the files.\n";
    }

    int depth1, depth2;
    vector<std::vector<std::complex<double>>> rho, sigma;

    // Read matrices in parallel from both files
    while (true) {
        bool ok1 = readNextMatrix(file1, depth1, rho);
        bool ok2 = readNextMatrix(file2, depth2, sigma);

        if (!ok1 || !ok2) break;

        if (depth1 != depth2) {
            std::cerr << "Depth mismatch: " << depth1 << " vs " << depth2 << "\n";
            break;
        }

        std::cout << "Processing Depth " << depth1 << "\n";
        // // 1. Method using the square root of rho
        // cout << "Matrix quest" << endl;
        // Eigen_matrix rho_mat(dim, dim);
        // for (int i = 0; i < dim; i++)
        // {
        //     for (int j = 0; j < dim; j++)
        //     {
        //         rho_mat(i,j) = rho[i][j];
        //     }
        // }
        //
        // Eigen_matrix sigma_mat(dim, dim);
        // cout << "Matrix qiskit" << endl;
        // for (int i = 0; i < dim; i++)
        // {
        //     for (int j = 0; j < dim; j++)
        //     {
        //         sigma_mat(i,j) = sigma[i][j];
        //     }
        // }
        // Eigen_matrix rho_sqrt = rho_mat.sqrt();
        //
        // Eigen_matrix fin_mat = rho_sqrt * sigma_mat* rho_sqrt;
        // Eigen_matrix fin_squared = fin_mat.sqrt();
        // Complex_value trace = fin_squared.trace();
        // double fidelity = norm(trace);
        //
        // cout << "Fidelity of the two matrices qiskit and quest for depth " << depth1 << ": " << fidelity << endl;

        // 2. Method using square root of the product rho*sigma
        vector<std::vector<std::complex<double>>> matrixproduct;
        mulMat(rho, sigma, matrixproduct);
        // get the eingenvalues of the matrix product
        Eigen_matrix eigen_mat(dim, dim);

        for (int i = 0; i < dim; i++)
        {
            for (int j = 0; j < dim; j++)
            {
                eigen_mat(i,j) = matrixproduct[i][j];
            }
        }

        EigenSolver ces;
        ces.compute(eigen_mat, false); // compute only eingenvalues, not eigenvectors

        Complex_value sum_eingvals_sqrt = 0; // som of the square roots
        for (int e = 0; e < dim; e++)
        {
            sum_eingvals_sqrt += sqrt(ces.eigenvalues()(e));
        }
        cout << "Fidelity of the two matrices qiskit and quest for depth " << depth1 << ": " << pow(sum_eingvals_sqrt.real(),2) << endl;
    }

};