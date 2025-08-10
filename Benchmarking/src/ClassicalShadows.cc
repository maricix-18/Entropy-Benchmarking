
/**
 * Classical Shadows Protocol class
 */
#include "ClassicalShadows.h"

map<pair<int,int>, vector<double>> beta_values()
{
    map<pair<int,int>, vector<double>> beta_values;
    
    for(int i = 0; i < 3; i++) //i=0/1/2 corresponds to H/HSdag/Id
        for(int j = 0; j < 3; j++) //j=0/1/2 corresponds to H/HSdag/Id
            {   
                pair<int, int> key = make_pair(i, j);
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

    int num_measurements = 3; // M
    int shots = 100; // k
    int groups = 2;

    int meas_set_per_group = ceil(double(num_measurements / groups));

    map<string, vector<double>> beta_values_map = beta_values();


    

    // create array of qubits
    vector<int> qubits_vector;
    for (int i = 0; i < qubits; i++)
    {
        qubits_vector.push_back(i);
    }
    
    for (int j = 0; j < num_measurements; j++)
    {
        // generate and get measurement setting
        vector<int> key = mydensitymatrix.getMeasurementSetting(qubits);
        // use clone to apply measurements
        Qureg clone = createCloneQureg(ds_qreg);

        cout << "Apply Measurement layer\n";
        mydensitymatrix.measurement(clone, qubits, backend);

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
        map<std::string, int> counts;
        
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
            std::cout << kv.first << ": " << kv.second << std::endl;
        }

    }
       
     
      

    cout << "Destroy Q Register\n";
    destroyQureg(ds_qreg);

}