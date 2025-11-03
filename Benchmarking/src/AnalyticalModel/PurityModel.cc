#include "PurityModel.h"

void PurityModel::initialise(Backend &_backend, int &qubits, int &max_depth) {
    // set backend
    backend = &_backend;

    // init values
    _qubits = qubits;
    _max_depth = max_depth;

    p1 = backend->get_p1_local();
    p2 = backend->get_p2_local();
};

void PurityModel::purityModel_globalDP_localDP()
{
    //done
    cout << "purityModel_globalDP of local depolarising probabilities" << endl;
    double alpha_1 = log(1/(1-p1));
    double alpha_2 = log(1/(1-p2));
    for (int d = 0; d <= _max_depth; d++)
    {
        double pur =  (1 - pow(2,(-_qubits)))*(exp(-2*(2*alpha_1*_qubits + alpha_2 * (_qubits - 1))* d) - 1) + 1;
        all_pur.push_back(pur);
        all_R2d.push_back(-1 * log2(pur) / _qubits);
    }
};

double PurityModel::purity_model_globalDP_CS_circuit_measerr(double &d, double &alpha_1, double &alpha_2, double &beta) {
    // done
    return (1 - pow(2,(-_qubits)))*(exp(-2*(alpha_1*_qubits*(2*d) + alpha_2*(_qubits-1)*d + beta*_qubits)) - 1) + 1;
};

double PurityModel::purity_model_globalDP_CS_circuit_measerr_part_eval(int &d, double &alpha_2, double &beta) {
    // done
    double c = p1/p2;
    double alpha_1 = alpha_2 * c;
    double dd = static_cast<double>(d);
    return purity_model_globalDP_CS_circuit_measerr( dd, alpha_1, alpha_2, beta);
};

vector<double> PurityModel::linspace(int start, int stop, int num) {
    vector<double> y;
    y.reserve(num);

    if (num == 0) {
        return y;
    }
    if (num == 1) {
        y.push_back(start);
        return y;
    }

    double div = num - 1;
    double delta = stop - start;
    double step = delta / div;

    for (int i = 0; i < num; ++i) {
        y.push_back(start + step * i);
    }

    if (num > 1) {
        y.back() = stop; // ensure exact endpoint
    }

    return y;
};

pair<Eigen::VectorXd, Eigen::MatrixXd> PurityModel::curve_fit_eigen(
    vector<int>& xdata,
    vector<double>& ydata,
    Eigen::VectorXd p0,
    Eigen::VectorXd& lb,
    Eigen::VectorXd& ub)
{
    PurityFitFunctor functor(*this, xdata, ydata);
    Eigen::NumericalDiff<PurityFitFunctor> numDiff(functor);
    Eigen::LevenbergMarquardt<Eigen::NumericalDiff<PurityFitFunctor>> lm(numDiff);

    lm.setXtol(1e-8);
    lm.setFtol(1e-8) ;
    lm.setMaxfev(2000) ;


    lm.minimize(p0);

    // Clamp to bounds
    for (int i = 0; i < p0.size(); ++i)
        p0[i] = clamp(p0[i], lb[i], ub[i]);

    // Compute Jacobian numerically
    Eigen::MatrixXd J(ydata.size(), p0.size());
    numDiff.df(p0, J);

    // Covariance estimation using pseudo-inverse for stability
    Eigen::JacobiSVD<Eigen::MatrixXd> svd(J.transpose() * J, Eigen::ComputeThinU | Eigen::ComputeThinV);
    double tol = 1e-12; // tolerance for small singular values
    Eigen::VectorXd singular_vals = svd.singularValues();
    Eigen::VectorXd inv_singular_vals = singular_vals.unaryExpr([tol](double x) { return (x > tol) ? 1.0 / x : 0.0; });
    Eigen::MatrixXd cov = svd.matrixV() * inv_singular_vals.asDiagonal() * svd.matrixU().transpose();


    return {p0, cov};
};

void PurityModel::purityModel_globalDP_CS()
{
    cout << "purityModel_globalDP_CS" << endl;
    double c = p1/p2;

    // 1. get short metrics for the experiment -
    ifstream file("../../Data_test/ClassicalShadows_metrics/Q3_m320_k1000_g5_s3.json");
    if (!file.is_open()) {
        cerr << "Error: could not open file.\n";
    }

    json j;
    file >> j;
    map<string, vector<double>> short_metrics_classim;
    for (auto& [key, value] : j.items()) {
        short_metrics_classim[key] = value.get<vector<double>>();
    }

    // 2. depth tab - vector [0,1,2..d]
    vector<int> depth_tab(_max_depth+1);
    iota(depth_tab.begin(), depth_tab.end(), 0);

    // 3. depth_tab_more_points np.linspace(depth_min, depth_max+1, 1000)
    depth_tab_more_points = linspace(0, _max_depth+1, 1000);

    // 4. fit curve based on data
    // popt_classim, _ = curve_fit(purity_model_globalDP_CS_circuit_measerr_part_eval, depth tab, short_metrics_classim, bounds=(0,1))
    Eigen::VectorXd p0(2);
    p0 << 0.5, 0.5; // initial guess

    Eigen::VectorXd lb(2), ub(2);
    lb << 0.0, 0.0;
    ub << 1.0, 1.0;

    auto [popt, pcov] = curve_fit_eigen(depth_tab, short_metrics_classim["all_pur_mean_diff_n"], p0, lb, ub);

    double alpha_1_optim_classim = popt[0] * c;
    double alpha_2_optim_classim = popt[0];
    double beta_optim_classim = popt[1];

    for (double d : depth_tab_more_points) {
        double pur = purity_model_globalDP_CS_circuit_measerr(d, alpha_1_optim_classim, alpha_2_optim_classim, beta_optim_classim);
        all_pur.push_back(pur);
        all_R2d.push_back(-1 * log2(pur) / _qubits);
    }

};

void PurityModel::saveMetrics()
{
    cout << "Saving Analytical model metrics to file." << endl;
    json j;
    string filename;
#ifdef _WIN32
    filename = "../../Data_test/AnalyticalModel_metrics/Q" + to_string(_qubits) + ".json";;
#else
    filename = "../Data_test/AnalyticalModel_metrics/Q" + to_string(_qubits) + ".json";;
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

    for (double pur : all_pur) {
        j["all_pur_diff_n"].push_back(pur);
    }
    for (double R2d : all_R2d) {
        j["all_R2d_diff_n"].push_back(R2d);
    }
    for (double dtbm : depth_tab_more_points) {
        j["depth_tab_more_points"].push_back(dtbm);
    }

    ofstream out(filename);
    if (out.is_open())
        out << setw(4) << j << endl;
};

// TODO
void PurityModel::purityModel_globalDP()
{
    // ?? What is this one?
    cout << "purityModel_globalDP"<< endl;
    // double alpha_1 = log(1/(1-p1));
    // double alpha_2 = log(1/(1-p2));
    // for (int d = 0; d <= _max_depth; d++)
    // {
    //     double pur =  (1 - pow(2,(-_qubits)))*(exp(-2*(2*alpha_1*_qubits + alpha_2 * (_qubits - 1))* d) - 1) + 1;
    //     all_pur.push_back(pur);
    // }
};


