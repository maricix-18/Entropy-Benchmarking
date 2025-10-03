#include "Swap.h"

void Swap::somefunc() { 
    cout << "Swap protocol function called." << endl;
    backend->some_backendfunc();
};

void Swap::metrics()
{
    cout << "Swap metrics function called for last depth." << endl;
    // add swap circuit
    swap_circuit();

    // for n samples do the protocol
    for (int n = 0; n < samples; n++)
    {
        vector<double> pur_means;

        for (int i = 0; i < groups; i++)
        {
            cout << "group: " << i << endl;
            // counts of measurements for current group
            for (int k = 0; k < shots; k++)
            {
                cout << "shot: " << k << endl;
                Qureg clone;
                clone = createCloneQureg(ds_qreg);

                // build bitstring for this shot
                string bitstring;
                for (int x = 0; x < ds_qreg.numQubits; x++)
                {
                    int out = applyQubitMeasurement(clone, x);
                    bitstring += to_string(out);
                }
                // counts are ordered by bitstring order
                counts[bitstring]++;
                destroyQureg(clone);
            }

            double pur_estimate = estimate_purity_from_swap_test(counts);
            pur_means.push_back(pur_estimate);

            //clear for each group
            counts.clear();
        }
        //for each sample

        double pur_mom = median(pur_means);
        double R2d_mom = (-1 * log2(pur_mom)) / _qubits;
            
        pur_samples.push_back(pur_mom);
        R2d_samples.push_back(R2d_mom);

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

double Swap::median(vector<double> &means) {
    sort(means.begin(), means.end());
    size_t n = means.size();

    if (n % 2 == 1) {
        // odd number of elements
        return means[n / 2];
    } else {
        // even number of elements
        return (means[n / 2 - 1] + means[n / 2]) / 2.0;
    }
};

string Swap::bitwise_AND(string& str1, string& str2) {
    // returns the bitwise AND of two strings
    string result;
    for (size_t i = 0; i < str1.length(); ++i) {
        if (str1[i] == '1' && str2[i] == '1') {
            result += '1';
        } else {
            result += '0';
        }
    }
    return result;
}

int Swap::parity_bit(string& bitstring) {
    // parity bit of a string
    int count = 0;
    for (char c : bitstring) {
        if (c == '1') {
            count++;
        }
    }
    return count % 2;
};

int Swap::swap_test_outcome(string &outcome)
{
    int num_qubits = outcome.length() / 2;

    string str1 = outcome.substr(0, num_qubits);
    string str2 = outcome.substr(num_qubits, 2*num_qubits);
    string bitwise = bitwise_AND(str1, str2);
    int result = parity_bit(bitwise);

    return result;
};

double Swap::estimate_purity_from_swap_test(map<string, int> &counts)
{
    int nb_outcome_0 = 0;
    int nb_outcome_1 = 0;

    for (auto it = counts.begin(); it != counts.end(); ++it) {
        string outcome = it->first;
        int nb_times_outcome = it->second;

        if (swap_test_outcome(outcome) == 0) {
            nb_outcome_0 += nb_times_outcome;
        } 
        else 
        {
            nb_outcome_1 += nb_times_outcome;
        }
    }   

    int nb_measurements = nb_outcome_0 + nb_outcome_1;
    double pur_estimate = (2.0 * nb_outcome_0 / nb_measurements) - 1.0;
    return pur_estimate;
};

void Swap::swap_circuit()
{
    // add swap circuit
    // populate the second half of the circuit
    int q2_start = _qubits;
    int q2_fin = (q2_start + _qubits);
    for (int i = 0; i < _depth; i++) {
        backend->applyLayer(ds_qreg, q2_start, q2_fin, angles_array);
    }

    cout << "Swap circuit " << endl;
    for (int i = 0; i < _qubits; i++) {
        applyControlledPauliX(ds_qreg, i, i + _qubits);
        applyHadamard(ds_qreg, i);
    }
};

void Swap::saveMetrics()
{
    cout << "Saving Swap metrics to file." << endl;
    json j;
    string filename = "../../Data_test/Swap_metrics/Q" + to_string(_qubits) + ".json";
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
        out << std::setw(4) << j << std::endl;
};

