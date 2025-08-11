
/**
 * Classical Shadows Protocol class
 */
#include "ClassicalShadows.h"
#include <random>
#include <fstream>
#include <iomanip>
#include "nlohmann/json.hpp"
using namespace std;
using json = nlohmann::ordered_json;

void append_to_json_CS(const std::string& filename, double& pur_mean,  double& pur_std, double& R2d_mean, double& R2d_std) 
{
    json j;

    // Read if file already exists
    if (std::filesystem::exists(filename)) {
        std::ifstream in(filename);
        if (in.is_open())
            in >> j;
    }

    // Append your data
    j["all_pur_mean_diff_n"].push_back(pur_mean);
    j["all_pur_std_diff_n"].push_back(pur_std);
    j["all_R2d_mean_diff_n"].push_back(R2d_mean);
    j["all_R2d_std_diff_n"].push_back(R2d_std);

    // Write back
    std::ofstream out(filename);
    if (out.is_open())
        out << std::setw(4) << j << std::endl;
}

void append_to_json_DM(const std::string& filename, double& pur, double& R2d) 
{
    json j;

    // Read if file already exists
    if (std::filesystem::exists(filename)) {
        std::ifstream in(filename);
        if (in.is_open())
            in >> j;
    }

    // Append your data
    j["all_pur_diff_n"].push_back(pur);
    j["all_R2d_diff_n"].push_back(R2d);

    // Write back
    std::ofstream out(filename);
    if (out.is_open())
        out << std::setw(4) << j << std::endl;
}

/**
 * For now, only consider at least 4 meas set and 2 groups
 */
map<string, vector<double>> beta_values()
{
   map<string, vector<double>> beta_values;
    
    for(int i = 0; i < 3; i++) //i=0/1/2 corresponds to H/HSdag/Id
        for(int j = 0; j < 3; j++) //j=0/1/2 corresponds to H/HSdag/Id
            {   
                string key = to_string(i) + to_string(j);
                if (i != j)
                    beta_values[key] = {1.0/2.0, 1.0/2.0}; //the first (resp. second) element corresponds to two identical (resp. different) outcomes
                else
                    beta_values[key] = {5.0, -4.0};
            }
           
    
    return beta_values;
}

void classicalshadows_protocol(int qubits, int depth, string backend)
{
    // filenames
    string filename_DM = "../Data/DensityMatrices_metrics/Q" + to_string(qubits) + "_D15.json";
    string filename_CS = "../Data/CS_metrics/Q" + to_string(qubits) + "_D15.json";
    
    cout << "Create Q Environment\n";
    if (isQuESTEnvInit() == 0)
      initQuESTEnv();
      
    cout << "Create Q Register\n";
    Qureg ds_qreg = createDensityQureg(qubits);
    Circuit<Layer> mydensitymatrix(qubits, depth);
    Qureg clone;
    cout << "Create Circuit\n";
    mydensitymatrix.createCircuit(ds_qreg, qubits, depth, backend);

    // get metrics for DM
    int dim = pow(2, qubits);
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
    double R2d = -1 * log2(pur) / qubits;

    append_to_json_DM(filename_DM, pur, R2d);

    double num_measurements = 320; // M
    int shots = 1000; // k
    int groups = 5;
    int samples = 3; // samples
    double K_factor = 1.0 / (shots * shots);

    int M_subgroup =static_cast<int>(ceil(double(num_measurements / groups)));
    double M_factor = 2.0 / (M_subgroup * (M_subgroup - 1));
    bool circuit_bool = true;
    bool verbose = true;

    vector<double> pur_samples;
    vector<double> R2d_samples;

    map<string, vector<double>> beta_values_map = beta_values();
    // multimap for duplicate keys
    multimap<vector<int>, map<string, int>> shadow_map;

    vector<int> key;
    vector<qreal> prob_dist;
    map<string, int> counts;
    vector<double> means;

    cout << fixed;
    cout << setprecision(17);
    // for n samples do this
    for (int n = 0; n < samples; n++)
    {
    
        cout << "Gather Shadows\n";

        for (int j = 0; j < num_measurements; j++)
        {
            //cout << "Measurment setting no: " << j <<endl;

            // generate and get measurement setting
            key = mydensitymatrix.getMeasurementSetting(qubits);
            
            // use clone to apply measurements
            clone = createCloneQureg(ds_qreg);

            //cout << "Apply Measurement layer\n";
            mydensitymatrix.measurement(clone, qubits, backend);

           // cout<<"Gen prob distribution\n";
            // get prob distribution
            long long dim = 1LL << qubits; // number of basis states
            for (long long i = 0; i < dim; i++) {
                qreal prob = calcProbOfBasisState(clone, i);
                prob_dist.push_back(prob);
                // print prob distribution
                // for (int q = qubits - 1; q >= 0; q--)
                //         std::cout << ((i >> q) & 1);
                //     std::cout << ": " << prob << std::endl;
            }
            // no need for clone anymore
            //cout << "Destroy Clone Q Register\n";
            destroyQureg(clone);

            // for sampling
            random_device rd;
            mt19937 gen(rd());
            discrete_distribution<> d(prob_dist.begin(), prob_dist.end());
            
            // run shots
            for (int k = 0; k < shots; k++)
            {
                // each shot samples a bitstring
                int sample = d(gen);
                string bitstring;
                for (int q = qubits - 1; q >= 0; --q)
                    bitstring += ((sample >> q) & 1) ? '1' : '0';
                counts[bitstring]++;
                
            }
            // results
            // for (const auto& kv : counts) {
            //     cout << kv.first << ": " << kv.second << std::endl;
            // }

            // store shadows
            shadow_map.insert({key,counts});
            prob_dist.clear();
            counts.clear();
            //shadow_map[key] = counts;
        }

        cout << "Size shadow map " << shadow_map.size() <<endl;

        // apply Classical Shadows protocol
        cout << "Apply Classical Shadows Protocol\n";

        cout<< "Subgroups: "<< M_subgroup << endl;

        for (int i = 0; i < groups; i++)
        {
           // cout << "Inside group: "<< i << endl;
            int start_idx = i * M_subgroup;
            int end_idx = (i + 1) * M_subgroup;

            int map_size = (int)shadow_map.size();
            if (end_idx > map_size) {
                end_idx = map_size;
            }

            // Create iterators to start and end positions
            auto start_it = shadow_map.begin();
            std::advance(start_it, start_idx);

            auto end_it = shadow_map.begin();
            std::advance(end_it, end_idx);

            // Now copy the range [start_it, end_it) into a vector of pairs
            vector<pair<vector<int>, map<string, int>>> shadow_list(start_it, end_it);

           // printing purposes 
            // if (verbose) {
            //    // cout << "\n Sublist of shadows considered is : \n";
            // for (const auto& kv : shadow_list) {
            //         cout << "[ ";
            //         for (auto q : kv.first) cout << q << " ";
            //         cout << "], { ";
            //         for (auto& inner_kv : kv.second) cout << inner_kv.first << ":" << inner_kv.second << " ";
            //         cout << "}\n";
            //     }
            // }

            double purity = 0.0;

            for (int m1 = 0; m1 < M_subgroup; m1++) {
                //if (verbose) cout << "========================================================\n";
                //if (verbose) cout << "Random unitary index m1 = " << m1 << endl;
                //cout << "Shadow list size " << shadow_list.size() << endl;

                const vector<int>& m_description_1 = shadow_list[m1].first;
                const map<string,int>& m_counts_1 = shadow_list[m1].second;

                for (int m2 = 0; m2 < m1; m2++) {
                    //if (verbose) cout << "m2 = " << m2 << endl;
                    const vector<int>& m_description_2 = shadow_list[m2].first;
                    const map<string,int>& m_counts_2 = shadow_list[m2].second;

                    double trace_prod_shadows = 0.0;

                    // beta vals
                    vector<vector<double>> beta_items(qubits);
                    for (int n = 0; n < qubits; ++n) {
                        string key = to_string(m_description_1[n]) + to_string(m_description_2[n]);
                        beta_items[n] = beta_values_map[key]; 
                    }

                    
                    for (const auto& m_counts_item_1 : m_counts_1) {
                        const string& outcome_1 = m_counts_item_1.first;
                        int nb_times_outcome_1 = m_counts_item_1.second;

                        for (const auto& m_counts_item_2 : m_counts_2) {
                            const string& outcome_2 = m_counts_item_2.first;
                            int nb_times_outcome_2 = m_counts_item_2.second;

                            double prod_over_num_qubits = 1.0;

                            for (int n = 0; n < qubits; ++n) {
                                int t = qubits - 1 - n;

                                char nth_bit_from_outcome_1, nth_bit_from_outcome_2;
                                if (circuit_bool) {
                                    nth_bit_from_outcome_1 = outcome_1[t];
                                    nth_bit_from_outcome_2 = outcome_2[t];
                                } else {
                                    nth_bit_from_outcome_1 = outcome_1[n];
                                    nth_bit_from_outcome_2 = outcome_2[n];
                                }

                                double beta;
                                if (nth_bit_from_outcome_1 == nth_bit_from_outcome_2)
                                    beta = beta_items[n][0];
                                else
                                    beta = beta_items[n][1];

                                prod_over_num_qubits *= beta;
                            }

                           // if (verbose) cout << "\n Product of interest :" << prod_over_num_qubits << endl;

                            trace_prod_shadows += nb_times_outcome_1 * nb_times_outcome_2 * prod_over_num_qubits;
                        }
                    }
                    //cout<<"Trace prod per subgroup: " << trace_prod_shadows<<endl;
                    purity += K_factor * trace_prod_shadows;
                   // cout << "Purity: "<<purity<<endl;
                }
            }
            purity *= M_factor;
            means.push_back(purity);

        }

        cout << "Final means values: "<<endl;
        for (auto m:means)
        {
            cout << m <<endl;
        }

        //median of means
        int mid = means.size() / 2;
        nth_element(means.begin(), means.begin() + mid, means.end());
        double mom = means[mid]; // purity val
        double R2d = (-1 * log2(mom)) / qubits; // R2d val

        cout << "Classical Shadows Protocol completed.\n";
        cout << "Median of means: " << mom << " R2d " << R2d << endl;
        // gether list of samples
        pur_samples.push_back(mom);
        R2d_samples.push_back(R2d);

        // clear 
        shadow_map.clear();
        means.clear();

    }// samples end
    // no need for register anymore
    cout << "Destroy Q Register\n";
    destroyQureg(ds_qreg);
    // Compute stats out of samples per depth

    // mean purity
    double sum = accumulate(pur_samples.begin(), pur_samples.end(), 0.0);
    double pur_mean = sum / pur_samples.size();

    // Std for purity
    double accum = 0.0;
    for (double x : pur_samples) {
        accum += (x - pur_mean) * (x - pur_mean);
    }
    double pur_std = sqrt(accum / pur_samples.size()); 

    // mean r2d
    sum = accumulate(R2d_samples.begin(), R2d_samples.end(), 0.0);
    double R2d_mean = sum / R2d_samples.size();

    //std for r2d
    accum = 0.0;
    for (double x : R2d_samples) {
        accum += (x - R2d_mean) * (x - R2d_mean);
    }
    double R2d_std = sqrt(accum / R2d_samples.size()); 

    append_to_json_CS(filename_CS, pur_mean, pur_std, R2d_mean, R2d_std);

}