#include "ClassicalShadows.h"

void  ClassicalShadows::somefunc() {
    backend->some_backendfunc();
};

void  ClassicalShadows::metrics()
{

    beta_vals_paulibasis(); // if pauli basis measurements are used 

    //for n samples do the protocol
    for (int n = 0; n < samples; n++)
    {
        cout << "Gather Shadows for sample: " << n <<endl;
        gatherShadows();

        cout << "Apply Classical Shadows Protocol\n";

        classicalShadows_protocol();

        double mom = median(means);// purity val

        double R2d = (-1 * log2(mom)) / _qubits; // R2d val

        cout << "Classical Shadows Protocol completed.\n";

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
};

void ClassicalShadows::classicalShadows_protocol() {
    // details are in this paper https://arxiv.org/pdf/2412.18007
    // noting the data referencing them
    // Eq. B19
    for (int i = 0; i < groups; i++)
    {
        // Get list of shadows of current group
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
        double purity = 0.0;

        // Basically you do a product between each measurement settings outcomes
        // with the other measurement settings outcome in the same group
        // Because we have pauli basis measurements, we have a beta_values lists
        // which already stores the possible values based on what pauli basis was used

        for (int m1 = 0; m1 < M_subgroup; m1++) {
            auto it_m1 = shadow_list.begin();
            advance(it_m1, m1);

            vector<int> m_description_1 = it_m1->first;
            map<string,int> m_counts_1 = it_m1->second;

            for (int m2 = 0; m2 < m1; m2++) {
                auto it_m2 = shadow_list.begin();
                advance(it_m2, m2);

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
                        trace_prod_shadows += nb_times_outcome_1 * nb_times_outcome_2 * prod_over_num_qubits;
                    }
                }
                purity += K_factor * trace_prod_shadows; // Eq. B7
            }
        }

        purity *= M_factor; // Eq. B19
        means.push_back(purity);
    }
};

// sampling method
int ClassicalShadows::binarySearchCDF(vector<double>& cdf, double value) {
    // Find the index where value would be inserted to keep order
    auto it = lower_bound(cdf.begin(), cdf.end(), value);
    if (it == cdf.end())
        return int(cdf.size() - 1);
    return int(distance(cdf.begin(), it));
};

void ClassicalShadows::gatherShadows() {

    // Random number generator setup
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> dis(0.0, 1.0);

    for (int j = 0; j < num_measurements; j++)
    {
        // generate and get measurement setting
        key = generate_measurement_setting();

        // Sampling
        Qureg clone;
        clone = createCloneQureg(ds_qreg);
        backend->measurementLayer(clone, _qubits, key);
        size_t dim = 1ULL << _qubits;  // 2^numQubits

        // Prepare probability vector and CDF
        vector<double> probabilities(dim);
        vector<double> cdf(dim);

        // Fill probability vector
        for (size_t i = 0; i < dim; i++) {
            probabilities[i] = calcProbOfBasisState(clone, i);
        }

        // Compute cumulative distribution function (CDF)
        cdf[0] = probabilities[0];
        for (size_t i = 1; i < dim; i++) {
            cdf[i] = cdf[i - 1] + probabilities[i];
        }

        // Count measurement outcomes
        for (size_t shot = 0; shot < shots; shot++) {
            double r = dis(gen);
            int outcome = binarySearchCDF(cdf, r);
            // Convert outcome to bitstring with leading zeros
            string bitstring;
            for (int i = 0; i < _qubits; i++) {
                bitstring += ((outcome >> i) & 1) ? '1' : '0';
            }
            counts[bitstring]++;
        }

        destroyQureg(clone);
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

    return measurement_setting;
};


double ClassicalShadows::median(vector<double> &vec) {
    int n = int(vec.size());
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

void ClassicalShadows::saveMetrics()
{
    cout << "Saving Classical shadows metrics to file." << endl;
    json j;
    string filename = "../../Data_test/ClassicalShadows_metrics/Q" + to_string(_qubits) +
                                                                "_m" + to_string((int)num_measurements) +
                                                                "_k"+ to_string(shots) +
                                                                "_g"+ to_string(groups) +
                                                                "_s"+ to_string(samples) +
                                                                    ".json";
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