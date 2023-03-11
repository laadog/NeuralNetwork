#include "../headers/Trainer.h"


Trainer::Trainer(Network model_t, int count_t, int checks_t, float mutationPercentage_t, float mutationOffset_t, void (*answer_t)(double*, double*), double (*miss_t)(double*, double*), void (*generation_t)(int, Network, double), void (*activation_t)(double&)){
    model = model_t;
    count = count_t;
    checks = checks_t;
    mutationPercentage = mutationPercentage_t;
    mutationOffset = mutationOffset_t;
    answer = answer_t;
    miss = miss_t;
    generation = generation_t;
    activation = activation_t;
}

Network Trainer::train(int generations){
    Network* networks = (Network*)malloc(sizeof(Network) * count);
    networks[0] = model;
    int closest;
    double smallestOffset;
    double offset = 0;
    int cellCount = 0;
    int maxDepth = 0;

    for(int i = 1; i < count; i++){
        networks[i] = model.mutate(0.5, 0.1);
    }

    for(int i = 0; i < model.layerCount; i++){
        cellCount += model.depths[i] * model.depths[i+1];
        if(model.depths[i] > maxDepth){maxDepth = model.depths[i];}
    }

    if(model.depths[model.layerCount] > maxDepth){maxDepth = model.depths[model.layerCount];}

    double* inputLayer = (double*)malloc(sizeof(double) * maxDepth);
    double* outputLayer = (double*)malloc(sizeof(double) * maxDepth);
    double* expected = (double*)malloc(sizeof(double) * (model.depths[model.layerCount]));
    int layerOffset;
    double* t;

    for(int i = 0; i < generations; i++){

        for(int j = 0; j < count; j++){
            offset = 0;
            for(int z = 0; z < checks; z++){

                answer(inputLayer, expected);
                layerOffset = 0;
                for(int y = 0; y < model.layerCount; y++){
                    if(y != 0){
                        layerOffset += model.depths[y-1] * model.depths[y];
                    }
                    for(int c = 0; c < model.depths[y+1]; c++){
                        outputLayer[c] = 0;
                        for(int n = 0; n < model.depths[y]; n++){
                            outputLayer[c] += inputLayer[n] * networks[j].layers[layerOffset + n*model.depths[y+1] + c];
                        }
                        activation(outputLayer[c]);
                    }
                    t = inputLayer;
                    inputLayer = outputLayer;
                    outputLayer = t;
                }
                offset += miss(inputLayer, expected);
            
            }
            offset /= checks;
            if(j == 0 || offset < smallestOffset){
                smallestOffset = offset;
                closest = j;
            }
        }

        generation(i, networks[closest], smallestOffset);

        for(int j = 0; j < count; j++){
            if(j != closest){
                for(int z = 0; z < cellCount; z++){
                    if(float(rand() % 100) / 100 < mutationPercentage){
                        networks[j].layers[z] = networks[closest].layers[z] + (1 -float(rand() % 100) / 50) * mutationOffset;
                    }else{
                        networks[j].layers[z] = networks[closest].layers[z];

                    }
                }
            }
        }
        
    }

    free(inputLayer);
    free(outputLayer);
    free(expected);
    return networks[closest];

}
