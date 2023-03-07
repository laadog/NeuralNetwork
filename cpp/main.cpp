#include <iostream>
#include <stdlib.h>
#include <time.h>
#include <chrono>

using namespace std;

void printArray(int arr[], int length){
    cout << "[";
    for(int i = 0; i < length-1; i++){
        cout << arr[i] << ",";
    }
    cout << arr[length-1] <<"]" << endl;
}

void printArray(float arr[], int length){
    cout << "[";
    for(int i = 0; i < length-1; i++){
        cout << arr[i] << ",";
    }
    cout << arr[length-1] <<"]" << endl;
}

void printArray(double arr[], int length){
    cout << "[";
    for(int i = 0; i < length-1; i++){
        cout << arr[i] << ",";
    }
    cout << arr[length-1] <<"]" << endl;
}

class Network{
    int inputs, outputs;
    int* depths;
    int layerCount;

    public:
        float* layers;
        Network(int inputs_t, int outputs_t){
            inputs = inputs_t;
            outputs = outputs_t;
            srand(time(NULL));
        }

        Network(){

        };

        void generate(int depths_t[], int size){

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
                layers[i] = rand() % 10;
                
            }
        }

        double* calculate(double input[]){
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
                }
                t = inputLayer;
                inputLayer = outputLayer;
                outputLayer = t;
            }
            free(outputLayer);
            return inputLayer;
        }

        Network mutate(float percentage, float offset){
            
            Network mutated = Network(inputs, outputs);
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


};


class Trainer{
    Network model;
    int count, checks;
    float mutationPercentage, mutationOffset;
    double* (*answer)();
    double (*miss)(double*, double*);

    public:
    Trainer(Network model_t, int count_t, int checks_t, float mutationPercentage_t, float mutationOffset_t){
        model = model_t;
        count = count_t;
        checks = checks_t;
        mutationPercentage = mutationPercentage_t;
        mutationOffset = mutationOffset_t;
    }

    Network train(int generations){
        Network* networks = (Network*)malloc(sizeof(Network) * count);

        networks[0] = model;
        for(int i = 1; i < count; i++){
            networks[i] = model.mutate(0.5, 0.1);
        }

        for(int i = 0; i < generations; i++){
            int closest;
            double smallestOffset;

            for(int j = 0; j < count; j++){
                double offset = 0;
                for(int z = 0; z < checks; z++){
                    double input[2] = {rand() % 10, rand() % 10};
                    double output[1] = {input[0] + input[1]};
                    double* answer = networks[j].calculate(input);
                    offset += abs(output[0] - answer[0]);
                }
                offset /= checks;
                if(j == 0 || offset < smallestOffset){
                    smallestOffset = offset;
                    closest = j;
                }
            }

            cout << "Gen: " << i << " offset: " << smallestOffset << endl;
            networks[0] = networks[closest];
            for(int j = 1; j < count; j++){
                networks[j] = networks[closest].mutate(mutationPercentage, mutationOffset);
            }
            
        }

        return networks[0];
    }

};


double* answer(){
    srand(time(NULL));
    double* prompt = (double*)malloc(sizeof(double)*3);
    prompt[0] = (rand() % 100) / 10;
    prompt[1] = (rand() % 100) / 10;
    prompt[2] = prompt[0] + prompt[1];
    return prompt;
}

double miss(double* input, double* output){
    return abs(input[2] - output[0]);
}

int main(int argc, char const *argv[])
{
    // Network n = Network(2, 1);
    // int depths[2] = {2,1};
    // n.generate(depths, 2);

    // Trainer t = Trainer(n, 10, 10, 0.5, 0.1); 
    // t.train(100);   
    printArray(answer(), 3);

    return 0;
}
