#include "Protocol.h"

void Protocol::setBackend(Backend &_backend) {
    cout << "Setting backend in protoc0l.cc." << endl;
    backend = &_backend;// Point the base class Backend pointer to the eg simulatr obj
};

void Protocol::somefunc()
{
    cout << "Protocol function called." << endl;
    backend->some_backendfunc();
};

void Protocol::init(int &qubits, int &depth)
{
    // init values
    this->_qubits = qubits;
    this->_depth = depth;

    cout << "Create Q Environment\n";
    if (isQuESTEnvInit() == 0)
        initQuESTEnv();
    
    cout << "Create Q Register\n";
    this->ds_qreg = createDensityQureg(_qubits);

    //generate the angles for the circuit
    angles_generator();

};

void Protocol::buildCircuit()
{
    for (int d = 0; d < this->_depth; d++)
    {
        backend->applyLayer(this->ds_qreg, this->_qubits, this->angles_array);
    }
    
};

void Protocol::measurement()
{

};
   
void Protocol::metrics()
{
    cout << "Protocol metrics function called." << endl;   
};

void Protocol::saveMetrics()
{
    
};

void Protocol::angles_generator()
{
    // set seed
    init_genrand(837); // Equivalent to np.random.seed(837)

    int angles_per_layer = 2*this->_qubits;
    int total_angles = angles_per_layer*this->_depth;

    cout << "Generating angles.\n";

    // get angle values
    for (int i = 0; i < total_angles; i++) 
    {
        double r = 2 * PI * genrand_res53();
        //printf("Random[%d] = %.17f\n", i, r);
        this->angles_array.push_back(r);
    }
};

void Protocol::destroy()
{
    cout << "Destroy Q Register\n";
    destroyQureg(this->ds_qreg);
};

// Protocol::~Protocol()
// {
//     cout << "Destroy Q Register\n";
//     destroyQureg(this->ds_qreg);
// };