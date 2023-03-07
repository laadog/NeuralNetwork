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
    float* layers;
    int* depths;
    int layerCount;

    public:
        Network(int inputs_t, int outputs_t){
            inputs = inputs_t;
            outputs = outputs_t;
        }

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
            srand(time(NULL));
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

            cout << sizeof(layers) << endl;

            return mutated;
        }


};

int main(int argc, char const *argv[])
{

    
    Network n = Network(2, 1);
    int depths[3] = {2,3,1};
    double input[2] = {1, 1};
    n.generate(depths, 3);

    printArray(n.calculate(input), 2);


    return 0;
}
