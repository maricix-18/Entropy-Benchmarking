/**
 * Density Matrix Protocol class
 */
#include "DensityMatrix.h"

void densitymatrix_protocol(int qubits, int depth, string backend)
{
    cout << "Create Q Environment\n";
      if (isQuESTEnvInit() == 0)
        initQuESTEnv();
      cout << "Create Q Register\n";
      Qureg ds_qreg = createDensityQureg(qubits);

      Circuit<Layer> mydensitymatrix(qubits, depth);

      mydensitymatrix.createCircuit(ds_qreg, qubits, depth, backend);
      
      cout << "Destroy Q Register\n";
      destroyQureg(ds_qreg);

}