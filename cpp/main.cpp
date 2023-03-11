#include <iostream>
#include <time.h>
#include <math.h>

#include "NeuralNetwork/NeuralNetwork.h"

using namespace std;

void answer(double* input, double* expected){
    input[0] = double(rand() % 100) / 10;
    input[1] = double(rand() % 100) / 10;
    expected[0] = input[0] + input[1];
    return;
}

double miss(double* output, double* expected){
    return abs(output[0] - expected[0]);
}

void generation(int index, Network n, double offset){
    //cout << "Gen: " << index << " offset: " << offset << endl;
}

void activation(double& input){
       //input = 1/(1+exp(-input));
}

int main(int argc, char const *argv[])
{

    srand(time(NULL));
    Network n = Network(2, 1);

    int depths[2] = {2,1};
    n.generate(depths, 2);

    Trainer t = Trainer(n, 10, 10, 0.5, 0.1, answer, miss, generation, activation); 

    Stopwatch s;

    s.start();
    
    t.train(100000);   

    s.stop();

    showMemoryUsage();
    
    return 0;
}
