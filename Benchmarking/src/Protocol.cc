#include "Protocol.h"

void Protocol::initialise(Backend &_backend, int &qubits, int &max_depth)
{
    // set backend
    setBackend(_backend);

    // init values
    _qubits = qubits;
    _max_depth = max_depth;

    // init angles
    angles_generator();
};

void Protocol::setBackend(Backend &_backend) {
    cout << "Setting backend in protoc0l.cc." << endl;
    backend = &_backend;// Point the base class Backend pointer to the eg simulatr obj
};

void Protocol::setProtocol(string &ptr)
{
    // for swap register
    this_prot = ptr;
};

void Protocol::setQureg()
{
    cout << "Create Q Environment\n";
    if (isQuESTEnvInit() == 0)
        initQuESTEnv();

    if (this_prot == "Swap")
    {
        cout << "Create Swap Register\n";
        ds_qreg = createDensityQureg(2*_qubits);
    }
    else{
        cout << "Create Q Register\n";
        ds_qreg = createDensityQureg(_qubits);
    }
}

void Protocol::buildCircuit(int &curr_depth)
{
    // build circuit for current depth
    _depth = curr_depth;

    if (_depth > 0)
    {   // only build layer if depth > 0
        int st_qubit = 0;
        if (this_prot == "Swap")
        {
            // populate q register for the swap test
            st_qubit = 0;
            int fn_qubit = _qubits;

            int st_swap = _qubits;
            int fn_swap = 2*_qubits;
            for (int d = 1; d <= _depth; d++)
            {
                backend->applyLayer(ds_qreg, st_qubit, fn_qubit, angles_array, d);
                backend->applyLayer(ds_qreg, st_swap, fn_swap, angles_array, d);
            }
        }
        else
        {
            int fn_qubit = _qubits;

            for (int d = 1; d <= _depth; d++)
            {
                backend->applyLayer(ds_qreg, st_qubit, fn_qubit, angles_array, d);
            }
        }
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
    // generate all the angle for up to max depth
    // set seed
    init_genrand(837); // Equivalent to np.random.seed(837)

    int angles_per_layer = 2*this->_qubits;
    int total_angles = angles_per_layer*_max_depth;
    cout << "Generating angles.\n";

    // get angle values
    for (int i = 0; i < total_angles; i++)
    {
        double r = 2 * PI * genrand_res53();
        this->angles_array.push_back(r);
    }
};

void Protocol::destroy()
{
    cout << "Destroy Q Register\n";
    destroyQureg(this->ds_qreg);
};

void Protocol::somefunc()
{
    cout << "Protocol function called." << endl;
    backend->some_backendfunc();
};
