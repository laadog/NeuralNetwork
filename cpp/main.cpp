#include <iostream>
#include <stdlib.h>
#include <time.h>

#include <chrono>
#include <fstream>
#include <unistd.h>

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

    public:
        float* layers;
        int layerCount;
        int* depths;
        Network(int inputs_t, int outputs_t){
            inputs = inputs_t;
            outputs = outputs_t;
        }

        Network(){
        };

        ~Network(){
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
    void (*answerC)(double*, double*);
    double (*miss)(double*, double*);

    public:
    Trainer(Network model_t, int count_t, int checks_t, float mutationPercentage_t, float mutationOffset_t, double* (*answer_t)(), void (*answerC_t)(double*, double*), double (*miss_t)(double*, double*)){
        model = model_t;
        count = count_t;
        checks = checks_t;
        mutationPercentage = mutationPercentage_t;
        mutationOffset = mutationOffset_t;
        answer = answer_t;
        answerC = answerC_t;
        miss = miss_t;
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
                    double* input = answer();
                    double* output = networks[j].calculate(input);
                    offset += miss(input, output);
                    free(input);
                    free(output);
                }
                offset /= checks;
                if(j == 0 || offset < smallestOffset){
                    smallestOffset = offset;
                    closest = j;
                }
            }

            //cout << "Gen: " << i << " offset: " << smallestOffset << endl;
            networks[0] = networks[closest];
            for(int j = 1; j < count; j++){
                networks[j] = networks[closest].mutate(mutationPercentage, mutationOffset);
            }
            
        }

        return networks[0];
    }

    Network trainBeta(int generations){
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

                    answerC(inputLayer, expected);
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

            //cout << "Gen: " << i << " offset: " << smallestOffset << endl;

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

};


double* answer(){
    double* prompt = (double*)malloc(sizeof(double)*3);
    prompt[0] = double(rand() % 100) / 10;
    prompt[1] = double(rand() % 100) / 10;
    prompt[2] = prompt[0] + prompt[1];
    return prompt;
}

void answerC(double* input, double* expected){
    input[0] = double(rand() % 100) / 10;
    input[1] = double(rand() % 100) / 10;
    expected[2] = input[0] + input[1];
    return;
}

double miss(double* input, double* output){
    return abs(input[0] - output[0]);
}

int main(int argc, char const *argv[])
{

    srand(time(NULL));
    Network n = Network(2, 1);

    int depths[2] = {2,1};
    n.generate(depths, 2);

    Trainer t = Trainer(n, 10, 10, 0.5, 0.1, &answer, &answerC, &miss); 



    chrono::steady_clock::time_point begin = chrono::steady_clock::now();
    
    t.trainBeta(100000);   

    chrono::steady_clock::time_point end = chrono::steady_clock::now();
    cout << "Time difference = " << chrono::duration_cast<std::chrono::milliseconds>(end - begin).count() << "[ms]" << endl;

    int tSize = 0, resident = 0, share = 0;
    ifstream buffer("/proc/self/statm");
    buffer >> tSize >> resident >> share;
    buffer.close();
    long page_size_kb = sysconf(_SC_PAGE_SIZE) / 1024; // in case x86-64 is configured to use 2MB pages
    double rss = resident * page_size_kb;
    double shared_mem = share * page_size_kb;
    cout << "Private Memory - " << rss - shared_mem << "kB\n";
    
    return 0;
}
