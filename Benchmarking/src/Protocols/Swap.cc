#include "Swap.h"

void Swap::somefunc() { 
    cout << "Swap protocol function called." << endl;
    backend->some_backendfunc();
};

int Swap::binarySearchCDF(const std::vector<double>& cdf, double value) {
    // Find the index where value would be inserted to keep order
    // This corresponds to the sampled outcome
    auto it = std::lower_bound(cdf.begin(), cdf.end(), value);
    if (it == cdf.end()) return cdf.size() - 1;
    return std::distance(cdf.begin(), it);
};

void Swap::metrics()
{
    cout << "Swap metrics function called for last depth." << endl;
    // add swap circuit
    swapLayer();

    // for n samples do the protocol
    for (int n = 0; n < samples; n++)
    {
        vector<double> pur_means;
      // Random number generator setup
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_real_distribution<> dis(0.0, 1.0);

        for (int i = 0; i < groups; i++)
        {
            cout << "group: " << i << endl;
            size_t dim = 1ULL << ds_qreg.numQubits;  // 2^numQubits

            // Prepare probability vector and CDF
            vector<double> probabilities(dim);
            vector<double> cdf(dim);

            // Fill probability vector
            for (size_t i = 0; i < dim; i++) {
                probabilities[i] = calcProbOfBasisState(ds_qreg, i);
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
                for (int i = 0; i < ds_qreg.numQubits; i++) {
                    bitstring += ((outcome >> i) & 1) ? '1' : '0';
                }
                counts[bitstring]++;
            }

            double pur_estimate = estimatePurityFromSwapTest(counts);
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
        accum += pow(abs(x - pur_mean), 2);
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
        accum += pow(abs(x - R2d_mean),2);
    }
    double R2d_std = sqrt(accum / R2d_samples.size());
    all_R2d_std.push_back(R2d_std);

    // clean up
    pur_samples.clear();
    R2d_samples.clear();
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

string Swap::bitwiseAND(string& str1, string& str2) {
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

int Swap::parityBit(string& bitstring) {
    // parity bit of a string
    int count = 0;
    for (char c : bitstring) {
        if (c == '1') {
            count++;
        }
    }
    return count % 2;
};

int Swap::swapTestOutcome(string &outcome)
{
    int num_qubits = outcome.length() / 2;

    string str1 = outcome.substr(0, num_qubits);
    string str2 = outcome.substr(num_qubits, 2*num_qubits);
    string bitwise = bitwiseAND(str1, str2);
    int result = parityBit(bitwise);

    return result;
};

double Swap::estimatePurityFromSwapTest(map<string, int> &counts)
{
    int nb_outcome_0 = 0;
    int nb_outcome_1 = 0;

    for (auto it = counts.begin(); it != counts.end(); ++it)
    {
        string outcome = it->first;
        int nb_times_outcome = it->second;

        if (swapTestOutcome(outcome) == 0) {
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

void Swap::swapLayer()
{
    cout << "Swap layer" << endl;
    for (int i = 0; i < _qubits; i++) {
        cout << "controlled x: control q" << i << " target q" << i + _qubits << endl;
        applyControlledPauliX(ds_qreg, i, i + _qubits);
        cout << "hadamard q" << i << endl;
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

