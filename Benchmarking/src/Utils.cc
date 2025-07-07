#include "Utils.h"

#define PI 3.14159265358979323846

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
