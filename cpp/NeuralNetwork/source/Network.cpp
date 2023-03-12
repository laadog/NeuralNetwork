#include "../headers/Network.h"


Network::Network(int inputs_t, int outputs_t, void(*activation_t)(double&)){
    inputs = inputs_t;
    outputs = outputs_t;
    activation = activation_t;
}

Network::Network(){
};

Network::~Network(){
}

void Network::generate(int depths_t[], int size){

    layerCount = size - 1;
    depths = (int*)malloc(sizeof(int)*size);
    int cellCount = 0;

    for(int i = 0; i < size-1; i++){
        depths[i] = depths_t[i];
        cellCount += depths_t[i] * depths_t[i+1];
    }
    depths[size-1] = depths_t[size-1];
    layers = (float*)malloc(sizeof(float)*cellCount);
    for(int i = 0; i < cellCount; i++){
        layers[i] = (1 - (rand() % 100) / 50)*1;
        
    }
}

double* Network::calculate(double* input){
    int maxDepth = 0;

    for(int i = 0; i < layerCount + 1; i++){
        if(depths[i] > maxDepth){maxDepth = depths[i];}
    }

    double* inputLayer = (double*)malloc(sizeof(double) * maxDepth);
    double* outputLayer = (double*)malloc(sizeof(double) * maxDepth);

    for(int i = 0; i<inputs; i++){
        inputLayer[i] = input[i];
    }

    int offset = 0;
    double* t;

    for(int i = 0; i < layerCount; i++){
        if(i != 0){
            offset += depths[i-1] * depths[i];
        }
        for(int j = 0; j < depths[i+1]; j++){
            outputLayer[j] = 0;
            for(int z = 0; z < depths[i]; z++){
                outputLayer[j] += inputLayer[z] * layers[offset + z*depths[i+1] + j];
            }
            activation(outputLayer[j]);
        }
        t = inputLayer;
        inputLayer = outputLayer;
        outputLayer = t;
    }
    free(outputLayer);
    return inputLayer;
}

Network Network::mutate(float percentage, float offset){
    
    Network mutated = Network(inputs, outputs, activation);
    mutated.layerCount = layerCount;
    mutated.depths = (int*)malloc(sizeof(int)*(layerCount+1));
    int cellCount = 0;

    for(int i = 0; i < layerCount; i++){
        mutated.depths[i] = depths[i];
        cellCount += depths[i] * depths[i+1];
    }

    mutated.depths[layerCount] = depths[layerCount];
    mutated.layers = (float*)malloc(sizeof(float)*cellCount);

    for(int i = 0; i < cellCount; i++){
        if((rand() % 100) / 100 < percentage){
            mutated.layers[i] = layers[i] + (1 -float(rand() % 100) / 50) * offset;
        }else{
            mutated.layers[i] = layers[i];
        }
        
    }
    return mutated;
}

int Network::getComplexity(){
    int count = 0;
    for(int i = 0; i < layerCount; i++){
        count += depths[i] * depths[i+1];
    }
    return count;
}
