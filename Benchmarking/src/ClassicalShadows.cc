
/**
 * Classical Shadows Protocol class
 */
#include "ClassicalShadows.h"
#include <random>
using namespace std;

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
    cout << "Create Q Environment\n";
    if (isQuESTEnvInit() == 0)
      initQuESTEnv();
      
    cout << "Create Q Register\n";
    Qureg ds_qreg = createDensityQureg(qubits);
    Circuit<Layer> mydensitymatrix(qubits, depth);

    cout << "Create Circuit\n";
    mydensitymatrix.createCircuit(ds_qreg, qubits, depth, backend);

    double num_measurements = 4; // M
    int shots = 100; // k
    int groups = 2;

    //int meas_set_per_group = ceil(double(num_measurements / groups));

    map<string, vector<double>> beta_values_map = beta_values();
    map<vector<int>,map<string, int>> shadow_map;
   
    cout << "Gather Shadows\n";

    for (int j = 0; j < num_measurements; j++)
    {
        // generate and get measurement setting
        vector<int> key = mydensitymatrix.getMeasurementSetting(qubits);
        
        // use clone to apply measurements
        Qureg clone = createCloneQureg(ds_qreg);

        cout << "Apply Measurement layer\n";
        mydensitymatrix.measurement(clone, qubits, backend);

        cout<<"Gen prob distribution\n";
        vector<qreal> prob_dist;
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
        cout << "Destroy Clone Q Register\n";
        destroyQureg(clone);

        // for sampling
        random_device rd;
        mt19937 gen(rd());
        discrete_distribution<> d(prob_dist.begin(), prob_dist.end());
        map<string, int> counts;
        
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
        for (const auto& kv : counts) {
            cout << kv.first << ": " << kv.second << std::endl;
        }

        // store shadows
        shadow_map[key] = counts;
    }
    // no need for register anymore
    cout << "Destroy Q Register\n";
    destroyQureg(ds_qreg);

    // apply Classical Shadows protocol
    cout << "Apply Classical Shadows Protocol\n";
    
    double K_factor = 1.0 / (shots * shots);
    int M_subgroup =static_cast<int>(ceil(double(num_measurements / groups)));
    double M_factor = 2.0 / (M_subgroup * (M_subgroup - 1));
    bool circuit_bool = true;
    bool verbose = true;

    vector<double> means;
    cout<< "Subgroups: "<<M_subgroup<<endl;

    for (int i=0; i< groups; i++)
    {
        cout << "Inside group: "<<i <<endl;
       // Extract subgroup of shadows as vector slice
        int start_idx = i * M_subgroup;
        int end_idx = start_idx + M_subgroup;
        if (end_idx > (int)shadow_map.size()) {
            end_idx = (int)shadow_map.size();
        }

        // get subgroup
        //vector< map<vector<int>,map<std::string, int>>> shadow_list(shadow_map.begin() + start_idx, shadow_map.begin() + end_idx);
        auto it_start = shadow_map.begin();
        advance(it_start, start_idx);

        auto it_end = shadow_map.begin();
        advance(it_end, end_idx);
        vector< std::pair<const std::vector<int>, std::map<std::string,int>> > shadow_list(it_start, it_end);

        // printing purposes 
         if (verbose) {
            cout << "\n Sublist of shadows considered is : \n";
           for (const auto& kv : shadow_list) {
                cout << "[ ";
                for (auto q : kv.first) cout << q << " ";
                cout << "], { ";
                for (auto& inner_kv : kv.second) cout << inner_kv.first << ":" << inner_kv.second << " ";
                cout << "}\n";
            }
        }

        double purity = 0.0;

        for (int m1 = 0; m1 < M_subgroup; ++m1) {
            if (verbose) cout << "========================================================\n";
            if (verbose) cout << "Random unitary index m1 = " << m1 << endl;

            const vector<int>& m_description_1 = shadow_list[m1].first;
            const map<string,int>& m_counts_1 = shadow_list[m1].second;
            for (int m2 = 0; m2 < m1; ++m2) {
                const vector<int>& m_description_2 = shadow_list[m2].first;
                const map<string,int>& m_counts_2 = shadow_list[m2].second;

                double trace_prod_shadows = 0.0;

                // Prepare beta_items vector of size num_qubits
                vector<vector<double>> beta_items(qubits);
                for (int n = 0; n < qubits; ++n) {
                    string key = to_string(m_description_1[n]) + to_string(m_description_2[n]);
                    beta_items[n] = beta_values_map[key]; // vector<double> of size 2
                }

                // Loop over measurement outcomes
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

                        if (verbose) cout << "\n Product of interest :" << prod_over_num_qubits << endl;

                        trace_prod_shadows += nb_times_outcome_1 * nb_times_outcome_2 * prod_over_num_qubits;
                    }
                }
                cout<<"Trace prod per subgroup: " << trace_prod_shadows<<endl;
                purity += K_factor * trace_prod_shadows;
                cout << "Purity: "<<purity<<endl;
            }
        }

        purity *= M_factor;
        means.push_back(purity);
    }   

    cout << "Final means values: "<<endl;
    for (auto pur:means)
    {
        cout << pur <<endl;
    }
     // Compute median of means
    int mid = means.size() / 2;
    nth_element(means.begin(), means.begin() + mid, means.end());
    double mom = means[mid];

    cout << "Classical Shadows Protocol completed.\n";
    cout << "Median of means: " << mom << endl;

}