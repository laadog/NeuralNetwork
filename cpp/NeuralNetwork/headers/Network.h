#pragma once
#include <stdlib.h>
#include <chrono>
#include <fstream>
#include <unistd.h>

class Network{
    int inputs, outputs;


    public:
        void (*activation)(double&);
        float* layers;
        int layerCount;
        int* depths;
        Network(int, int, void (*)(double&));

        Network();

        ~Network();

        void generate(int*, int);
        double* calculate(double*);
        Network mutate(float, float);
        int getComplexity();
};