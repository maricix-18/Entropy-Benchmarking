#include "ClassicalShadows.h"

void  ClassicalShadows::somefunc() { 
    cout << "classicalShadows protocol function called." << endl;
    backend->some_backendfunc();
};

double ClassicalShadows::median(vector<double> &vec) {
    int n = vec.size();
    sort(vec.begin(), vec.end());
    if (n % 2 == 1) {
        // odd number
        return vec[n / 2];
    }
    else {
        // even number
        return (vec[n / 2 - 1] + vec[n / 2]) / 2.0;
    }
};

void  ClassicalShadows::metrics()
{
    cout << fixed;
    cout << setprecision(17);

    beta_vals_paulibasis(); // if pauli basis measurements are used 

    //for n samples do the protocol
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
            cout << m << endl;
        }


        double mom = median(means);// purity val

        double R2d = (-1 * log2(mom)) / _qubits; // R2d val

        cout << "Classical Shadows Protocol completed.\n";

        cout << "Median of means (mom): " << mom << " R2d " << R2d << endl;

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
    double variance = 0.0;
    for (double x : pur_samples) {
        variance += pow(abs(x - pur_mean), 2);
    }
    double pur_std = sqrt(variance / pur_samples.size());
    all_purity_std.push_back(pur_std);

    // mean r2d
    sum = accumulate(R2d_samples.begin(), R2d_samples.end(), 0.0);
    double R2d_mean = sum / R2d_samples.size();
    all_R2d_mean.push_back(R2d_mean);

    //std for r2d
    variance = 0.0;
    for (double x : R2d_samples) {
        variance += pow(abs(x - R2d_mean),2);
    }
    double R2d_std = sqrt(variance / R2d_samples.size());
    all_R2d_std.push_back(R2d_std);

    // clean up
    pur_samples.clear();
    R2d_samples.clear();

   // // print shadow map
   //  for (auto a: shadow_map) {
   //      cout << "key: ";
   //      for (auto b : a.first) {
   //          cout << b;
   //      }
   //
   //      cout << " counts: ";
   //      for (auto c : a.second) {
   //          cout << c.first << " " << c.second << " ";
   //      }
   //      cout << endl;
   //  }
};

void ClassicalShadows::classicalShadows_protocol() {
    // details are in this paper https://arxiv.org/pdf/2412.18007
    // noting the data referencing them
    // Eq. B19
    for (int i = 0; i < groups; i++)
    {
        // Get list of shadows of current group
        cout << "Inside group: "<< i << endl;
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
        Shadow_map shadow_list(start_it, end_it);
        cout << "Shadow list size for group " << i  << ": " << shadow_list.size() << endl;
        double purity = 0.0;
        // for (auto a: shadow_list) {
        //     cout << "key: ";
        //     for (auto b : a.first) {
        //         cout << b;
        //     }
        //
        //     cout << " counts: ";
        //     for (auto c : a.second) {
        //         cout << c.first << " " << c.second << " ";
        //     }
        //     cout << endl;
        // }
        // Basically you do a product between each measurement settings outcomes
        // with the other measurement settings outcome in the same group
        // Because we have pauli basis measurements, we have a beta_values lists
        // which already stores the possible values based on what pauli basis was used

        for (int m1 = 0; m1 < M_subgroup; m1++) {
            //if (verbose) cout << "Random unitary index m1 = " << m1 << endl;
            //cout << "Shadow list size " << shadow_list.size() << endl;
            auto it_m1 = shadow_list.begin();
            advance(it_m1, m1);

            vector<int> m_description_1 = it_m1->first;
            map<string,int> m_counts_1 = it_m1->second;

            for (int m2 = 0; m2 < m1; m2++) {
                auto it_m2 = shadow_list.begin();
                advance(it_m2, m2);
                //if (verbose) cout << "m2 = " << m2 << endl;
                vector<int> m_description_2 = it_m2->first;
                map<string,int> m_counts_2 = it_m2->second;

                double trace_prod_shadows = 0.0;

                // beta vals
                vector<vector<double>> beta_items(_qubits);
                for (int n = 0; n < _qubits; ++n) {
                    string key = to_string(m_description_1[n]) + to_string(m_description_2[n]);
                    beta_items[n] = beta_values[key]; // (Table I)
                }
                for (auto m_counts_item_1 : m_counts_1) {
                    string outcome_1 = m_counts_item_1.first;
                    int nb_times_outcome_1 = m_counts_item_1.second;

                    for (auto m_counts_item_2 : m_counts_2) {
                        string outcome_2 = m_counts_item_2.first;
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
        cout << "Measurement setting no: " << j <<endl;

        // generate and get measurement setting
        key = generate_measurement_setting();

        for (int k = 0; k < shots; k++)
        {
            Qureg clone;
            clone = createCloneQureg(ds_qreg);
            backend->measurementLayer(clone, _qubits, key);
            // build bitstring for this shot
            string bitstring;
            // try big end as qiskit
            for (int x = _qubits-1; x >= 0; x--)
            {
                int out = applyQubitMeasurement(clone, x);
                bitstring += to_string(out);
            }
            // counts are ordered by bitstring order
            counts[bitstring]++;
            destroyQureg(clone);
        }

        // store shadows
        //print counts
        // cout<<"Counts: ";
        // for (auto x :counts) {
        //     cout<< x.first <<": "<< x.second <<endl;
        // }

        // reverse key for qiskit
        reverse(key.begin(), key.end());
        // shadow_map.insert({key,counts});
        shadow_map.push_back(make_pair(key, counts));
        counts.clear();
        key.clear();
    }
};

void ClassicalShadows::beta_vals_paulibasis()
{
    for(int i = 0; i < 3; i++) //i=0/1/2 corresponds to H/HSdag/Id
        for(int j = 0; j < 3; j++) //j=0/1/2 corresponds to H/HSdag/Id
            {   
                string key = to_string(i) + to_string(j);
                if (i != j)
                    beta_values[key] = {1.0/2.0, 1.0/2.0}; //first - identical outcomes. second - different
                else
                    beta_values[key] = {5.0, -4.0};
            }
};

vector<int> ClassicalShadows::generate_measurement_setting()
{
    vector<int> measurement_setting;

    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> dis(0, 2);

    for (int i = 0; i < _qubits; i++) {
        int basis = dis(gen);
        measurement_setting.push_back(basis);
    }
    cout << "Meas setting: ";
    for (int i = 0; i < _qubits; i++) {
        cout << measurement_setting[i] << ", ";
    }
    cout << endl;
    return measurement_setting;
};

void ClassicalShadows::saveMetrics()
{
    cout << "Saving Classical shadows metrics to file." << endl;
    json j;
    string filename = "../../Data_test/ClassicalShadows_metrics/Q" + to_string(_qubits) + ".json";
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