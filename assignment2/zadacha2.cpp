#include <iostream>
#include <cstdlib>
#include <ctime>
#include <chrono>
#include <climits>
#include <omp.h>

using namespace std;
using namespace chrono;

int main() {
    const int N = 10000;
    int* arr = new int[N];

    srand((unsigned)time(nullptr));

    // 1. Заполнение массива случайными числами
    for (int i = 0; i < N; i++) {
        arr[i] = rand() % 100000;
    }

    // 2. Последовательный поиск min и max
    auto startSeq = high_resolution_clock::now();

    int minSeq = arr[0];
    int maxSeq = arr[0];

    for (int i = 1; i < N; i++) {
        if (arr[i] < minSeq) minSeq = arr[i];
        if (arr[i] > maxSeq) maxSeq = arr[i];
    }

    auto endSeq = high_resolution_clock::now();
    double timeSeq =
        duration<double, milli>(endSeq - startSeq).count();

    // 3. Параллельный поиск min и max (OpenMP)
    auto startPar = high_resolution_clock::now();

    int minPar = INT_MAX;
    int maxPar = INT_MIN;

    #pragma omp parallel for reduction(min:minPar) reduction(max:maxPar)
    for (int i = 0; i < N; i++) {
        if (arr[i] < minPar) minPar = arr[i];
        if (arr[i] > maxPar) maxPar = arr[i];
    }

    auto endPar = high_resolution_clock::now();
    double timePar =
        duration<double, milli>(endPar - startPar).count();

    // 4. Вывод результатов
    cout << "Array size: " << N << endl;
    cout << "Sequential min: " << minSeq << ", max: " << maxSeq << endl;
    cout << "Parallel   min: " << minPar << ", max: " << maxPar << endl;

    cout << "Sequential time: " << timeSeq << " ms" << endl;
    cout << "Parallel   time: " << timePar << " ms" << endl;

    if (timePar > 0) {
        cout << "Speedup: " << timeSeq / timePar << "x" << endl;
    }

    delete[] arr;
    return 0;
}



