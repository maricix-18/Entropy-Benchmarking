#include "PurityModel.h"

void PurityModel::initialise(Backend &_backend, int &qubits, int &max_depth) {
    // set backend
    backend = &_backend;

    // init values
    _qubits = qubits;
    _max_depth = max_depth;

    p1_local = backend->get_p1_local();
    p2_local = backend->get_p2_local();
};

void PurityModel::depth_tab_populate() {
    // if empty populate depth tab - vector [0,1,2..d]
    if (depth_tab.empty()) {
        depth_tab.resize(_max_depth+1);
        iota(depth_tab.begin(), depth_tab.end(), 0);
    }
}

void PurityModel::depth_tab_more_points_populate() {
    // if empty populate
    if (depth_tab_more_points.empty()) {
        depth_tab_more_points = linspace(0, _max_depth, 1000);
    }
}

double PurityModel::purityModel_globalDP_value(double &depth, double &alpha_1, double &alpha_2) {
    return (1 - pow(2,(-_qubits)))*(exp(-2*(2*alpha_1*_qubits + alpha_2 * (_qubits - 1))* depth) - 1) + 1;
}

double PurityModel::purityModel_globalDP_R2d_model_part_eval(double &depth, double &alpha_2) {
    double c = p1_local/p2_local;
    double alpha_1 = alpha_2 * c;
    double R2d = -1 * log2(purityModel_globalDP_value(depth, alpha_1, alpha_2)) / _qubits;
    return R2d;
}

void PurityModel::purityModel_globalDP()
{
    cout << "purityModel_globalDP"<< endl;
    // get short metrics for the experiment -- density matrix values
    string file_path = find_file_DM();
    ifstream file(file_path);

    if (!file.is_open()) {
        cerr << "Error: could not open file.\n";
    }

    json j;
    file >> j;
    map<string, vector<double>> all_R2d_results;
    for (auto& [key, value] : j.items()) {
        all_R2d_results[key] = value.get<vector<double>>();
    }

    double c = p1_local/p2_local;
    depth_tab_populate();
    depth_tab_more_points_populate();

    // fit curve based on data
    Eigen::VectorXd p0(2);
    p0 << 0.5, 0.5; // initial guess

    Eigen::VectorXd lb(2), ub(2);
    lb << 0.0, 0.0; // lower bounds
    ub << 1.0, 1.0; // upper bounds

    int params_to_fit = 1;
    name_model = "purity_model_globalDP";
    auto [popt, pcov] = curve_fit_eigen(depth_tab, all_R2d_results["all_R2d_diff_n"], p0,lb,ub, params_to_fit);

    double alpha_1_optim_classim = popt[0] * c;
    double alpha_2_optim_classim = popt[0];
    fitted_params.push_back(make_pair("alpha_1_optim_classim", alpha_1_optim_classim));
    fitted_params.push_back(make_pair("alpha_2_optim_classim", alpha_2_optim_classim));
    for (double d : depth_tab_more_points) {
        double pur = purityModel_globalDP_value(d, alpha_1_optim_classim, alpha_2_optim_classim);
        double R2d = -1 * log2(pur)/ _qubits;
        all_pur.push_back(pur);
        all_R2d.push_back(R2d);
    }

};

double PurityModel::purityModel_globalDP_localDP_value(double &depth, double &p_1, double &p_2) {
    double alpha_1 = log(1/(1-p_1));
    double alpha_2 = log(1/(1-p_2));

    double pur =  (1 - pow(2,(-_qubits)))*(exp(-2*(2*alpha_1*_qubits + alpha_2 * (_qubits - 1))* depth) - 1) + 1;
    return pur;
}

double PurityModel::purityModel_globalDP_localDP_R2d_model_part_eval(double &depth, double &p_2) {
    double c = p1_local/p2_local;
    double p_1 = p_2 * c;
    double R2d = -1 * log2(purityModel_globalDP_localDP_value(depth,p_1, p_2)) / _qubits;
    return R2d;
}

void PurityModel::purityModel_globalDP_localDP()
{
    cout << "purityModel_globalDP_localDP"<< endl;
    // get short metrics for the experiment -- density matrix values
    string file_path = find_file_DM();
    ifstream file(file_path);
    if (!file.is_open()) {
        cerr << "Error: could not open file.\n";
    }

    json j;
    file >> j;
    map<string, vector<double>> all_R2d_results;
    for (auto& [key, value] : j.items()) {
        all_R2d_results[key] = value.get<vector<double>>();
    }

    depth_tab_populate();
    depth_tab_more_points_populate();
    double c = p1_local/p2_local;
    // fit curve based on data
    Eigen::VectorXd p0(2);
    p0 << 0.5, 0.5; // initial guess

    Eigen::VectorXd lb(2), ub(2);
    lb << 0.0, 0.0; // lower bounds
    ub << 1.0, 1.0; // upper bounds

    int params_to_fit = 1;
    name_model = "purity_model_globalDP_localDP";
    auto [popt, pcov] = curve_fit_eigen(depth_tab, all_R2d_results["all_R2d_diff_n"], p0,lb,ub, params_to_fit);

    double alpha_1_optim_classim = popt[0] * c;
    double alpha_2_optim_classim = popt[0];
    fitted_params.push_back(make_pair("alpha_1_optim_classim", alpha_1_optim_classim));
    fitted_params.push_back(make_pair("alpha_2_optim_classim", alpha_2_optim_classim));

    for (double d : depth_tab_more_points) {
        double pur = purityModel_globalDP_localDP_value(d, alpha_1_optim_classim, alpha_2_optim_classim);
        double R2d = -1 * log2(pur)/ _qubits;
        all_pur.push_back(pur);
        all_R2d.push_back(R2d);
    }
};

double PurityModel::purity_model_globalDP_CS_circuit_measerr(double &d, double &alpha_1, double &alpha_2, double &beta) {
    // done
    return (1 - pow(2,(-_qubits)))*(exp(-2*(alpha_1*_qubits*(2*d) + alpha_2*(_qubits-1)*d + beta*_qubits)) - 1) + 1;
};

double PurityModel::purity_model_globalDP_CS_circuit_measerr_part_eval(double &d, double &alpha_2, double &beta) {
    // done
    double c = p1_local/p2_local;
    double alpha_1 = alpha_2 * c;
    return purity_model_globalDP_CS_circuit_measerr( d, alpha_1, alpha_2, beta);
};


void PurityModel::purityModel_globalDP_CS()
{
    cout << "purityModel_globalDP_CS" << endl;
    double c = p1_local/p2_local;

    depth_tab_populate();
    depth_tab_more_points_populate();

    // get short metrics for the experiment
    string file_path = find_file_CS();
    ifstream file(file_path);
    if (!file.is_open()) {
        cerr << "Error: could not open file.\n";
    }

    json j;
    file >> j;
    map<string, vector<double>> short_metrics_classim;
    for (auto& [key, value] : j.items()) {
        short_metrics_classim[key] = value.get<vector<double>>();
    }

    Eigen::VectorXd p0(2);
    p0 << 0.5, 0.5; // initial guess

    Eigen::VectorXd lb(2), ub(2);
    lb << 0.0, 0.0;
    ub << 1.0, 1.0;

    int params_to_fit = 2;
    name_model = "purity_model_globalDP_CS";
    auto [popt, pcov] = curve_fit_eigen(depth_tab, short_metrics_classim["all_pur_mean_diff_n"], p0,lb,ub, params_to_fit);

    double alpha_1_optim_classim = popt[0] * c;
    double alpha_2_optim_classim = popt[0];
    double beta_optim_classim = popt[1];
    fitted_params.push_back(make_pair("alpha_1_optim_classim", alpha_1_optim_classim));
    fitted_params.push_back(make_pair("alpha_2_optim_classim", alpha_2_optim_classim));
    fitted_params.push_back(make_pair("beta_optim_classim", beta_optim_classim));

    for (double d : depth_tab_more_points) {
        double pur = purity_model_globalDP_CS_circuit_measerr(d, alpha_1_optim_classim, alpha_2_optim_classim, beta_optim_classim);
        all_pur.push_back(pur);
        all_R2d.push_back(-1 * log2(pur) / _qubits);
    }

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
        y.back() = stop; // ensure endpoint
    }

    return y;
};

pair<Eigen::VectorXd, Eigen::MatrixXd> PurityModel::curve_fit_eigen(vector<double>& xdata,vector<double>& ydata,Eigen::VectorXd p0,Eigen::VectorXd& lb,Eigen::VectorXd& ub, int &params_to_fit)
{
    PurityFitFunctor functor(*this, xdata, ydata, params_to_fit);
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

    // Covariance estimation
    Eigen::JacobiSVD<Eigen::MatrixXd> svd(J.transpose() * J, Eigen::ComputeThinU | Eigen::ComputeThinV);
    double tol = 1e-12; // tolerance for small singular values
    Eigen::VectorXd singular_vals = svd.singularValues();
    Eigen::VectorXd inv_singular_vals = singular_vals.unaryExpr([tol](double x) { return (x > tol) ? 1.0 / x : 0.0; });
    Eigen::MatrixXd cov = svd.matrixV() * inv_singular_vals.asDiagonal() * svd.matrixU().transpose();


    return {p0, cov};
};

string PurityModel::find_file_CS() {

    string directory = "../../Data_test/ClassicalShadows_metrics/"; // Directory to search in

    // Iterate over the files in the directory
    for (const auto& entry : filesystem::directory_iterator(directory)) {
        string filename = entry.path().filename().string();

        // Check if the filename starts with 'Q' followed by the _qubits number and ends with '.json'
        if (filename.substr(0, 1) == "Q" && filename.substr(1, std::to_string(_qubits).size()) == std::to_string(_qubits) &&
            filename.substr(filename.size() - 5) == ".json")
        {
            return filename;
        }
    }
    return "No matching file found.";
};

string PurityModel::find_file_DM() {
    string file_path = "../../Data_test/DensityMatrices_metrics/Q"+ to_string(_qubits) +".json";
    return file_path;
}

void PurityModel::saveMetrics()
{
    cout << "Saving Analytical model metrics to file." << endl;
    json j;
    string filename;

#ifdef _WIN32
    filename = "../../Data_test/AnalyticalModel_metrics/Q" + to_string(_qubits) + "_"+ name_model+".json";;
#else
    filename = "../Data_test/AnalyticalModel_metrics/Q" + to_string(_qubits) + "_"+ name_model+".json";;
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
    for (auto pair: fitted_params) {
        string key = pair.first;
        double value = pair.second;
        j[key].push_back(value);
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

    all_pur.clear();
    all_R2d.clear();
    depth_tab_more_points.clear();
    depth_tab.clear();
    fitted_params.clear();

};



