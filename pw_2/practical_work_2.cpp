#include <iostream>
#include <cstdlib>
#include <ctime>
#include <chrono>
#include <omp.h>

using namespace std;
using namespace chrono;

/* =====================================================
   ВСПОМОГАТЕЛЬНЫЕ ФУНКЦИИ
   ===================================================== */

void fillArray(int* arr, int n) {
    for (int i = 0; i < n; i++)
        arr[i] = rand() % 10000;
}

void copyArray(int* src, int* dest, int n) {
    for (int i = 0; i < n; i++)
        dest[i] = src[i];
}

/* =====================================================
   BUBBLE SORT
   ===================================================== */

void bubbleSortSeq(int* arr, int n) {
    for (int i = 0; i < n - 1; i++) {
        for (int j = 0; j < n - i - 1; j++) {
            if (arr[j] > arr[j + 1])
                swap(arr[j], arr[j + 1]);
        }
    }
}

void bubbleSortPar(int* arr, int n) {
    for (int i = 0; i < n - 1; i++) {
        #pragma omp parallel for
        for (int j = 0; j < n - i - 1; j++) {
            if (arr[j] > arr[j + 1])
                swap(arr[j], arr[j + 1]);
        }
    }
}

/* =====================================================
   SELECTION SORT
   ===================================================== */

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

/* =====================================================
   INSERTION SORT
   ===================================================== */

void insertionSortSeq(int* arr, int n) {
    for (int i = 1; i < n; i++) {
        int key = arr[i];
        int j = i - 1;
        while (j >= 0 && arr[j] > key) {
            arr[j + 1] = arr[j];
            j--;
        }
        arr[j + 1] = key;
    }
}

// Формально параллельная версия (эффективность низкая — это НОРМАЛЬНО)
void insertionSortPar(int* arr, int n) {
    #pragma omp parallel for
    for (int i = 1; i < n; i++) {
        int key = arr[i];
        int j = i - 1;
        while (j >= 0 && arr[j] > key) {
            arr[j + 1] = arr[j];
            j--;
        }
        arr[j + 1] = key;
    }
}

/* =====================================================
   MAIN
   ===================================================== */

int main() {
    srand((unsigned)time(nullptr));

    int sizes[] = {1000, 10000, 100000};

    for (int s = 0; s < 3; s++) {
        int N = sizes[s];
        cout << "\n==============================\n";
        cout << "Array size: " << N << endl;

        int* original = new int[N];
        int* arr = new int[N];

        fillArray(original, N);

        // ---------- BUBBLE SORT ----------
        copyArray(original, arr, N);
        auto t1 = high_resolution_clock::now();
        bubbleSortSeq(arr, N);
        auto t2 = high_resolution_clock::now();

        copyArray(original, arr, N);
        auto t3 = high_resolution_clock::now();
        bubbleSortPar(arr, N);
        auto t4 = high_resolution_clock::now();

        cout << "Bubble sort seq: "
             << duration<double, milli>(t2 - t1).count() << " ms\n";
        cout << "Bubble sort par: "
             << duration<double, milli>(t4 - t3).count() << " ms\n";

        // ---------- SELECTION SORT ----------
        copyArray(original, arr, N);
        t1 = high_resolution_clock::now();
        selectionSortSeq(arr, N);
        t2 = high_resolution_clock::now();

        copyArray(original, arr, N);
        t3 = high_resolution_clock::now();
        selectionSortPar(arr, N);
        t4 = high_resolution_clock::now();

        cout << "Selection sort seq: "
             << duration<double, milli>(t2 - t1).count() << " ms\n";
        cout << "Selection sort par: "
             << duration<double, milli>(t4 - t3).count() << " ms\n";

        // ---------- INSERTION SORT ----------
        copyArray(original, arr, N);
        t1 = high_resolution_clock::now();
        insertionSortSeq(arr, N);
        t2 = high_resolution_clock::now();

        copyArray(original, arr, N);
        t3 = high_resolution_clock::now();
        insertionSortPar(arr, N);
        t4 = high_resolution_clock::now();

        cout << "Insertion sort seq: "
             << duration<double, milli>(t2 - t1).count() << " ms\n";
        cout << "Insertion sort par: "
             << duration<double, milli>(t4 - t3).count() << " ms\n";

        delete[] original;
        delete[] arr;
    }

    return 0;
}
