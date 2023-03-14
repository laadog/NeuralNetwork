#include <iostream>
#include <time.h>
#include <math.h>
#include <fstream>
#include <sstream>
#include <string>

#include "NeuralNetwork/NeuralNetwork.h"

//using namespace std;
using std::cout;
using std::fstream;
using std::stringstream;



int* data;

void answer(double* input, double* expected){
    int offset = (random() % 1000) * 785;
    for(int i = 0; i < 784; i++){
        input[i] = data[offset + i];
        // input[i] = data[offset + i];
    }
    expected[0] = data[offset + 784];
    return;
}

double miss(double* output, double* expected){
    double highest = 0;
    int index;
    for(int i = 0; i < 10; i++){
        if(output[i] > highest){
            highest = output[i];
            index = i;
        }
    }

    if(index == expected[0]){
        return 0;
    }else{
        return 1;
    }
}

void generation(int index, Network n, double offset){
    cout << "Gen: " << index << " offset: " << offset << std::endl;
}

void activation(double& input){
       //input = 1/(1+exp(-input));
}

int main(int argc, char const *argv[])
{

    data = (int*)(malloc(sizeof(int)* 785 * 999));
    fstream fin;
    fin.open("data/mnistTrain.csv", std::ios::in);
    std::string temp, word;
    fin >> temp;
    int index = 0;
    while(fin >> temp){
        stringstream s(temp);
        while (getline(s, word, ',')) {
            data[index++] = stoi(word);
        }
    }

    srand(time(NULL));
    Network n = Network(784, 10, activation);

    int depths[3] = {784,64,10};
    n.generate(depths, 3);

    Trainer t = Trainer(n, 100, 1000, 0.25, 1, answer, miss, generation); 

    Stopwatch s;

    s.start();
    
    t.train(10, 8);   

    s.stop();

    showMemoryUsage();
    
    return 0;
}
