#pragma once
#include <stdlib.h>
#include <chrono>
#include <fstream>
#include <unistd.h>

class Network{
    int inputs, outputs;

    public:
        float* layers;
        int layerCount;
        int* depths;
        Network(int inputs_t, int outputs_t);

        Network();

        ~Network();

        void generate(int*, int );
        double* calculate(double*);
        Network mutate(float, float);
};