#pragma once

#include <iostream>
#include <chrono>
#include <fstream>
#include <unistd.h>

void printArray(int*, int );

void printArray(float*, int);

void printArray(double*, int);

class Stopwatch{
    std::chrono::steady_clock::time_point begin;
    public:
        void start();
        void stop();
};

void showMemoryUsage();