#include "../headers/Trainer.h"


Trainer::Trainer(Network model_t, int count_t, int checks_t, float mutationPercentage_t, float mutationOffset_t, void (*answer_t)(double*, double*), double (*miss_t)(double*, double*), void (*generation_t)(int, Network, double)){
    model = model_t;
    count = count_t;
    checks = checks_t;
    mutationPercentage = mutationPercentage_t;
    mutationOffset = mutationOffset_t;
    answer = answer_t;
    miss = miss_t;
    generation = generation_t;
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
                        model.activation(outputLayer[c]);
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

void threadFunc(Network* networks, int offset, int count, int checks, double& smallestOffset, int& index, void (*answer)(double*, double*), double (*miss)(double*, double*)){
    int closest;
    double smallestOffsetThread;
    int cellCount = 0;
    int maxDepth = 0;
    double inaccuracy;

    for(int i = 0; i < networks[0].layerCount; i++){
        cellCount += networks[0].depths[i] * networks[0].depths[i+1];
        if(networks[0].depths[i] > maxDepth){maxDepth = networks[0].depths[i];}
    }

    if(networks[0].depths[networks[0].layerCount] > maxDepth){maxDepth = networks[0].depths[networks[0].layerCount];}

    double* inputLayer = (double*)malloc(sizeof(double) * maxDepth);
    double* outputLayer = (double*)malloc(sizeof(double) * maxDepth);
    double* expected = (double*)malloc(sizeof(double) * (networks[0].depths[networks[0].layerCount]));
    int layerOffset;
    double* t;

    for(int j = offset; j < offset + count; j++){
            inaccuracy = 0;
            for(int z = 0; z < checks; z++){

                answer(inputLayer, expected);
                layerOffset = 0;
                for(int y = 0; y < networks[0].layerCount; y++){
                    if(y != 0){
                        layerOffset += networks[0].depths[y-1] * networks[0].depths[y];
                    }
                    for(int c = 0; c < networks[0].depths[y+1]; c++){
                        outputLayer[c] = 0;
                        for(int n = 0; n < networks[0].depths[y]; n++){
                            outputLayer[c] += inputLayer[n] * networks[j].layers[layerOffset + n*networks[0].depths[y+1] + c];
                        }
                        networks[0].activation(outputLayer[c]);
                    }
                    t = inputLayer;
                    inputLayer = outputLayer;
                    outputLayer = t;
                }
                inaccuracy += miss(inputLayer, expected);
            
            }
            inaccuracy /= checks;
            if(j == offset || inaccuracy < smallestOffsetThread){
                smallestOffsetThread = inaccuracy;
                closest = j;
            }
        }

        free(inputLayer);
        free(outputLayer);
        free(expected);
        if(index == -1 || smallestOffset > smallestOffsetThread){
            smallestOffset = smallestOffsetThread;
            index = closest;
        }
}


Network Trainer::train(int generations, int threads){
    Network* networks = (Network*)malloc(sizeof(Network) * count);
    networks[0] = model;
    std::thread workers[threads];
    int closest = -1;
    double smallestOffset;
    int cellCount = 0;
    int maxDepth = 0;
    int countPerThread = count / threads;

    for(int i = 1; i < count; i++){
        networks[i] = model.mutate(mutationPercentage, mutationOffset);
    }

    for(int i = 0; i < model.layerCount; i++){
        cellCount += model.depths[i] * model.depths[i+1];
        if(model.depths[i] > maxDepth){maxDepth = model.depths[i];}
    }

    if(model.depths[model.layerCount] > maxDepth){maxDepth = model.depths[model.layerCount];}

    for(int i = 0; i < generations; i++){

        int c = 0;
        closest = -1;
        for(int j = 0; j < threads; j++){
            if(j < count % threads){
                workers[j] = std::thread(threadFunc, std::ref(networks), j * countPerThread + c, countPerThread + 1, checks, std::ref(smallestOffset), std::ref(closest), answer, miss);
                c++;
            }else{
                workers[j] = std::thread(threadFunc, std::ref(networks), j * countPerThread + c, countPerThread , checks, std::ref(smallestOffset), std::ref(closest), std::ref(answer), std::ref(miss));
            }
        }

        for(int j = 0; j < threads; j++){
            workers[j].join();
        }

        generation(i , networks[closest], smallestOffset);

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
    
    return networks[closest];

}

int Trainer::getComplexity(){
    return model.getComplexity()*count*checks;
}