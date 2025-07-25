
#include "Utils.h"


void angles_generator(int qubits, int depth, vector<double> &angles_array)
{
    // set seed
    init_genrand(837); // Equivalent to np.random.seed(837)

    int angles_per_layer = 2*qubits;
    int total_angles = angles_per_layer*depth;

    std::cout << "Generating angles.\n";

    // get angle values
    for (int i = 0; i < total_angles; i++) {
        double r = 2 * PI * genrand_res53();
        //printf("Random[%d] = %.17f\n", i, r);
        angles_array.push_back(r);
    }
}

void generate_measurement_setting(vector<int> &measurement_setting, int qubits)
{
    // measurement setting
    for (int i = 0; i < qubits; i++) {
        int basis = genrand_int32() % 3; // 0, 1, or 2 (X ,Y, Z basis)
        //cout << "Measurement setting for qubit " << i << ": " << basis << "\n";
        measurement_setting.push_back(basis);
    }
}