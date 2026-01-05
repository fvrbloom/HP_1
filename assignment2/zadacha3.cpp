#include <iostream>
#include <cstdlib>
#include <ctime>
#include <chrono>
#include <omp.h>

using namespace std;
using namespace chrono;

/* ===============================
   ВСПОМОГАТЕЛЬНЫЕ ФУНКЦИИ
   =============================== */

void fillArray(int* arr, int n) {
    for (int i = 0; i < n; i++)
        arr[i] = rand() % 100000;
}

void copyArray(int* src, int* dest, int n) {
    for (int i = 0; i < n; i++)
        dest[i] = src[i];
}

/* ===============================
   SELECTION SORT (SEQUENTIAL)
   =============================== */

void selectionSortSeq(int* arr, int n) {
    for (int i = 0; i < n - 1; i++) {
        int minIdx = i;
        for (int j = i + 1; j < n; j++) {
            if (arr[j] < arr[minIdx])
                minIdx = j;
        }
        swap(arr[i], arr[minIdx]);
    }
}

/* ===============================
   SELECTION SORT (PARALLEL)
   =============================== */

void selectionSortPar(int* arr, int n) {
    for (int i = 0; i < n - 1; i++) {
        int minIdx = i;

        #pragma omp parallel for
        for (int j = i + 1; j < n; j++) {
            if (arr[j] < arr[minIdx]) {
                #pragma omp critical
                {
                    if (arr[j] < arr[minIdx])
                        minIdx = j;
                }
            }
        }
        swap(arr[i], arr[minIdx]);
    }
}

/* ===============================
   MAIN
   =============================== */

int main() {
    srand((unsigned)time(nullptr));

    int sizes[] = {1000, 10000};

    for (int s = 0; s < 2; s++) {
        int N = sizes[s];
        cout << "\n=============================\n";
        cout << "Array size: " << N << endl;

        int* original = new int[N];
        int* arr = new int[N];

        fillArray(original, N);

        // Sequential version
        copyArray(original, arr, N);
        auto t1 = high_resolution_clock::now();
        selectionSortSeq(arr, N);
        auto t2 = high_resolution_clock::now();

        double timeSeq =
            duration<double, milli>(t2 - t1).count();

        // Parallel version
        copyArray(original, arr, N);
        auto t3 = high_resolution_clock::now();
        selectionSortPar(arr, N);
        auto t4 = high_resolution_clock::now();

        double timePar =
            duration<double, milli>(t4 - t3).count();

        cout << "Sequential time: " << timeSeq << " ms\n";
        cout << "Parallel   time: " << timePar << " ms\n";

        if (timePar > 0) {
            cout << "Speedup: " << timeSeq / timePar << "x\n";
        }

        delete[] original;
        delete[] arr;
    }

    return 0;
}
