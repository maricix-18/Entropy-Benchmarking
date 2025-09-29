#include "ClassicalShadows.h"

void  ClassicalShadows::somefunc() { 
    cout << "classicalShadows protocol function called." << endl;
    backend->some_backendfunc();
};

void  ClassicalShadows::metrics()
{
    cout << fixed;
    cout << setprecision(17);

    beta_vals_paulibasis(); // if pauli basis measurements are used 

    // for n samples do the protocol
    for (int n = 0; n < samples; n++)
    {
        cout << "Gather Shadows for sample: " << n <<endl;;
        gatherShadows();

        cout << "Size shadow map " << shadow_map.size() <<endl;

        // apply Classical Shadows protocol
        cout << "Apply Classical Shadows Protocol\n";
        cout<< "Subgroups: "<< M_subgroup << endl;

        classicalShadows_protocol();

        cout << "Final means values: "<<endl;
        for (auto m:means)
        {
            cout << m <<endl;
        }

        //median of means - get the middle val
        int mid = int(means.size() / 2);
        nth_element(means.begin(), means.begin() + mid, means.end());
        double mom = means[mid]; // purity val
        double R2d = (-1 * log2(mom)) / _qubits; // R2d val

        cout << "Classical Shadows Protocol completed.\n";

        cout << "Median of means: " << mom << " R2d " << R2d << endl;

        // gether list of samples
        pur_samples.push_back(mom);
        R2d_samples.push_back(R2d);

        // clear 
        shadow_map.clear();
        means.clear();

    }// samples end

    // Compute metrics
    // mean purity
    double sum = accumulate(pur_samples.begin(), pur_samples.end(), 0.0);
    double pur_mean = sum / pur_samples.size();
    all_purity_mean.push_back(pur_mean);

    // Std for purity
    double accum = 0.0;
    for (double x : pur_samples) {
        accum += (x - pur_mean) * (x - pur_mean);
    }
    double pur_std = sqrt(accum / pur_samples.size());
    all_purity_std.push_back(pur_std);

    // mean r2d
    sum = accumulate(R2d_samples.begin(), R2d_samples.end(), 0.0);
    double R2d_mean = sum / R2d_samples.size();
    all_R2d_mean.push_back(R2d_mean);

    //std for r2d
    accum = 0.0;
    for (double x : R2d_samples) {
        accum += (x - R2d_mean) * (x - R2d_mean);
    }
    double R2d_std = sqrt(accum / R2d_samples.size());
    all_R2d_std.push_back(R2d_std);

};

void ClassicalShadows::classicalShadows_protocol() { 
    // details are in this paper https://arxiv.org/pdf/2412.18007
    // noting the data referencing them
    // Eq. B19 
    for (int i = 0; i < groups; i++)
    {
        // Get list of shadows of current group
        // cout << "Inside group: "<< i << endl;
        int start_idx = i * M_subgroup;
        int end_idx = (i + 1) * M_subgroup;

        int map_size = (int)shadow_map.size();
        if (end_idx > map_size) {
            end_idx = map_size;
        }

        auto start_it = shadow_map.begin();
        advance(start_it, start_idx);

        auto end_it = shadow_map.begin();
        advance(end_it, end_idx);

        // store the respective group in a list
        Shadow_list shadow_list(start_it, end_it);

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
        // Basically you do a product between each measurement settings outcomes
        // with the other measurement settings outcome in the same group
        // Because we have pauli basis measurements, we have a beta_values lists 
        // which already stores the possible values based on what pauli basis was used 
        for (int m1 = 0; m1 < M_subgroup; m1++) {
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
                vector<vector<double>> beta_items(_qubits);
                for (int n = 0; n < _qubits; ++n) {
                    string key = to_string(m_description_1[n]) + to_string(m_description_2[n]);
                    beta_items[n] = beta_values[key]; // (Table I)
                }

                
                for (const auto& m_counts_item_1 : m_counts_1) {
                    const string& outcome_1 = m_counts_item_1.first;
                    int nb_times_outcome_1 = m_counts_item_1.second;

                    for (const auto& m_counts_item_2 : m_counts_2) {
                        const string& outcome_2 = m_counts_item_2.first;
                        int nb_times_outcome_2 = m_counts_item_2.second;

                        double prod_over_num_qubits = 1.0;

                        for (int n = 0; n < _qubits; ++n) {
                            char nth_bit_from_outcome_1, nth_bit_from_outcome_2;
                            nth_bit_from_outcome_1 = outcome_1[n];
                            nth_bit_from_outcome_2 = outcome_2[n];

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
                // cout<<"Trace prod per subgroup: " << trace_prod_shadows<<endl;
                purity += K_factor * trace_prod_shadows; // Eq. B7
                // cout << "Purity: "<<purity<<endl;
            }
        }
        purity *= M_factor; // Eq. B19
        means.push_back(purity);
    }
};


void ClassicalShadows::gatherShadows() { 

    for (int j = 0; j < num_measurements; j++)
        {
            //cout << "Measurment setting no: " << j <<endl;

            // generate and get measurement setting
            key = generate_measurement_setting();
            
            // use clone to apply measurements
            Qureg clone;
            clone = createCloneQureg(ds_qreg);
            backend->measurementLayer(clone, _qubits, key);

            // get prob distribution of current measurement setting
            long long dim = 1LL << _qubits; // number of basis states
            for (long long i = 0; i < dim; i++) {
                qreal prob = calcProbOfBasisState(clone, i);
                prob_dist.push_back(prob);
                // print prob distribution
                // for (int q = qubits - 1; q >= 0; q--)
                //         std::cout << ((i >> q) & 1);
                //     std::cout << ": " << prob << std::endl;
            }
            // no need for clone anymore
            // cout << "Destroy Clone Q Register\n";
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
                for (int q = 0; q < _qubits; q++)
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
        }
};

void ClassicalShadows::beta_vals_paulibasis()
{
    for(int i = 0; i < 3; i++) //i=0/1/2 corresponds to H/HSdag/Id
        for(int j = 0; j < 3; j++) //j=0/1/2 corresponds to H/HSdag/Id
            {   
                string key = to_string(i) + to_string(j);
                if (i != j)
                    beta_values[key] = {1.0/2.0, 1.0/2.0}; //two identical outcomes
                else
                    beta_values[key] = {5.0, -4.0}; // different outcomes
            }
};

vector<int> ClassicalShadows::generate_measurement_setting()
{
    vector<int> measurement_setting;
    // measurement setting
    for (int i = 0; i < _qubits; i++) {
        int basis = genrand_int32() % 3; // 0, 1, or 2 (X ,Y, Z basis)
        //cout << "Measurement setting for qubit " << i << ": " << basis << "\n";
        measurement_setting.push_back(basis);
    }
    return measurement_setting;
};

void ClassicalShadows::saveMetrics()
{
    cout << "Saving Classical shadows metrics to file." << endl;
    json j;
    string filename = "../Data_test/ClassicalShadows_metrics/Q" + to_string(_qubits) + ".json";
    // check file or create
    struct stat buffer;
    if (stat(filename.c_str(), &buffer) == 0) {
        ifstream in(filename);
        if (in.is_open()) {
            cout << "File opened for reading.\n";
        }
    } else {
        ofstream out(filename);
        if (out.is_open()) {
            cout << "File created.\n";
        }
    }

    for (double pur_mean : all_purity_mean) {
         j["all_pur_mean_diff_n"].push_back(pur_mean);
    }
    for (double R2d_mean : all_R2d_mean) {
        j["all_R2d_mean_diff_n"].push_back(R2d_mean);
    }
    for (double pur_std : all_purity_std) {
       j["all_pur_std_diff_n"].push_back(pur_std);
    }
    for (double R2d_std : all_R2d_std) {
        j["all_R2d_std_diff_n"].push_back(R2d_std);
    }
    
    ofstream out(filename);
    if (out.is_open())
        out << setw(4) << j << endl;
};