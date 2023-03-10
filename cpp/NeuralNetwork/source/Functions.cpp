#include "../headers/Functions.h"

void printArray(int arr[], int length){
    std::cout << "[";
    for(int i = 0; i < length-1; i++){
        std::cout << arr[i] << ",";
    }
    std::cout << arr[length-1] <<"]" << std::endl;
}

void printArray(float arr[], int length){
    std::cout << "[";
    for(int i = 0; i < length-1; i++){
        std::cout << arr[i] << ",";
    }
    std::cout << arr[length-1] <<"]" << std::endl;
}

void printArray(double arr[], int length){
    std::cout << "[";
    for(int i = 0; i < length-1; i++){
        std::cout << arr[i] << ",";
    }
    std::cout << arr[length-1] <<"]" << std::endl;
}

void Stopwatch::start(){
    begin = std::chrono::steady_clock::now();
}

void Stopwatch::stop(){
    std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
    std::cout << "Elapsed: " << std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count() << "[ms]" << std::endl;
}

void showMemoryUsage(){
    int tSize = 0, resident = 0, share = 0;
    std::ifstream buffer("/proc/self/statm");
    buffer >> tSize >> resident >> share;
    buffer.close();
    long page_size_kb = sysconf(_SC_PAGE_SIZE) / 1024;
    double rss = resident * page_size_kb;
    double shared_mem = share * page_size_kb;
    std::cout << "Private Memory: " << rss - shared_mem << "kB\n";
}