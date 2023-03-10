#pragma once

#include "Network.h"

class Trainer{
    Network model;

    int count, checks;
    float mutationPercentage, mutationOffset;
    void (*answer)(double*, double*);
    double (*miss)(double*, double*);
    void (*generation)(int, Network, double);

    public:
    Trainer(Network,int ,int ,float, float, void (*)(double*, double*), double (*)(double*, double*), void (*)(int, Network, double));

    Network train(int);

};