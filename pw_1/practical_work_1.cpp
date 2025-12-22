
// practical_work_1.cpp
// Practical Work 1: Arrays, pointers and OpenMP
#include <iostream>
#include <chrono>
#include <cstdlib>
#include <ctime>
#include <omp.h>
using namespace std;

// Sequential average calculation
double calculateAverageSequential(int* array, int size) {
    long long sum = 0;
    for (int i = 0; i < size; ++i) {
        sum += array[i];
    }
    return static_cast<double>(sum) / size;
}

// Parallel average calculation
double calculateAverageParallel(int* array, int size) {
    long long sum = 0;
    #pragma omp parallel for reduction(+:sum)
    for (int i = 0; i < size; ++i) {
        sum += array[i];
    }
    return static_cast<double>(sum) / size;
}

int main() {
    int N;
    cout << "Enter array size N: ";
    cin >> N;
    
    if (N <= 0) {
        cout << "Invalid array size\n";
        return 1;
    }

    // Dynamic memory allocation
    int* data = new int[N];
    srand(static_cast<unsigned>(time(nullptr)));
    
    // Fill array with values [1..100]
    for (int i = 0; i < N; ++i) {
        data[i] = rand() % 100 + 1;
    }

    // Output array (only for small arrays)
    if (N <= 100) {
        cout << "Array elements:\n";
        for (int i = 0; i < N; ++i) {
            cout << data[i] << " ";
        }
        cout << "\n\n";
    } else {
        cout << "Array created with " << N << " elements.\n\n";
    }

    // -------------------------------
    // Part 1: min/max
    // -------------------------------
    
    // Sequential
    auto t1 = chrono::high_resolution_clock::now();
    int minSeq = data[0];
    int maxSeq = data[0];
    for (int i = 1; i < N; ++i) {
        if (data[i] < minSeq) minSeq = data[i];
        if (data[i] > maxSeq) maxSeq = data[i];
    }
    auto t2 = chrono::high_resolution_clock::now();
    chrono::duration<double, milli> timeSeq = t2 - t1;

    // Parallel
    int minPar = data[0];
    int maxPar = data[0];
    auto t3 = chrono::high_resolution_clock::now();
    #pragma omp parallel for reduction(min:minPar) reduction(max:maxPar)
    for (int i = 0; i < N; ++i) {
        if (data[i] < minPar) minPar = data[i];
        if (data[i] > maxPar) maxPar = data[i];
    }
    auto t4 = chrono::high_resolution_clock::now();
    chrono::duration<double, milli> timePar = t4 - t3;

    cout << "=== Min/Max Search ===" << endl;
    cout << "Sequential: min = " << minSeq
         << ", max = " << maxSeq
         << ", time = " << timeSeq.count() << " ms\n";
    cout << "Parallel:   min = " << minPar
         << ", max = " << maxPar
         << ", time = " << timePar.count() << " ms\n";
    cout << "Speedup: " << timeSeq.count() / timePar.count() << "x\n\n";

    // -------------------------------
    // Part 3: average value
    // -------------------------------
    
    // Sequential average
    auto t5 = chrono::high_resolution_clock::now();
    double avgSeq = calculateAverageSequential(data, N);
    auto t6 = chrono::high_resolution_clock::now();
    chrono::duration<double, milli> timeAvgSeq = t6 - t5;

    // Parallel average
    auto t7 = chrono::high_resolution_clock::now();
    double avgPar = calculateAverageParallel(data, N);
    auto t8 = chrono::high_resolution_clock::now();
    chrono::duration<double, milli> timeAvgPar = t8 - t7;

    cout << "=== Average Calculation ===" << endl;
    cout << "Sequential: avg = " << avgSeq 
         << ", time = " << timeAvgSeq.count() << " ms\n";
    cout << "Parallel:   avg = " << avgPar 
         << ", time = " << timeAvgPar.count() << " ms\n";
    cout << "Speedup: " << timeAvgSeq.count() / timeAvgPar.count() << "x\n";

    // Free memory
    delete[] data;
    
    return 0;
}