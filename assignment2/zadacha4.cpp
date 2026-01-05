#include <iostream>
#include <cuda_runtime.h>
#include <chrono>

using namespace std;
using namespace chrono;

#define BLOCK_SIZE 256

/* =====================================
   GPU KERNEL: SORT SUBARRAYS (ODD-EVEN)
   ===================================== */

__global__ void localSort(int* data, int n) {
    int tid = threadIdx.x;
    int start = blockIdx.x * blockDim.x;

    if (start + tid < n) {
        for (int i = 0; i < blockDim.x; i++) {
            int idx = start + tid;
            if (idx + 1 < n && data[idx] > data[idx + 1]) {
                int tmp = data[idx];
                data[idx] = data[idx + 1];
                data[idx + 1] = tmp;
            }
            __syncthreads();
        }
    }
}

/* =====================================
   CPU MERGE (FINAL MERGE STEP)
   ===================================== */

void merge(int* arr, int l, int m, int r) {
    int n1 = m - l + 1;
    int n2 = r - m;

    int* L = new int[n1];
    int* R = new int[n2];

    for (int i = 0; i < n1; i++) L[i] = arr[l + i];
    for (int j = 0; j < n2; j++) R[j] = arr[m + 1 + j];

    int i = 0, j = 0, k = l;

    while (i < n1 && j < n2) {
        if (L[i] <= R[j]) arr[k++] = L[i++];
        else arr[k++] = R[j++];
    }

    while (i < n1) arr[k++] = L[i++];
    while (j < n2) arr[k++] = R[j++];

    delete[] L;
    delete[] R;
}

void mergeSortCPU(int* arr, int l, int r) {
    if (l < r) {
        int m = l + (r - l) / 2;
        mergeSortCPU(arr, l, m);
        mergeSortCPU(arr, m + 1, r);
        merge(arr, l, m, r);
    }
}

/* =====================================
   MAIN
   ===================================== */

int main() {
    int sizes[] = {10000, 100000};

    for (int s = 0; s < 2; s++) {
        int N = sizes[s];
        cout << "\n=============================\n";
        cout << "Array size: " << N << endl;

        int* h_data = new int[N];
        for (int i = 0; i < N; i++)
            h_data[i] = rand() % 100000;

        int* d_data;
        cudaMalloc(&d_data, N * sizeof(int));
        cudaMemcpy(d_data, h_data, N * sizeof(int), cudaMemcpyHostToDevice);

        dim3 block(BLOCK_SIZE);
        dim3 grid((N + BLOCK_SIZE - 1) / BLOCK_SIZE);

        // GPU sort (local blocks)
        auto gpuStart = high_resolution_clock::now();
        localSort<<<grid, block>>>(d_data, N);
        cudaDeviceSynchronize();
        auto gpuEnd = high_resolution_clock::now();

        cudaMemcpy(h_data, d_data, N * sizeof(int), cudaMemcpyDeviceToHost);

        // CPU merge
        auto mergeStart = high_resolution_clock::now();
        mergeSortCPU(h_data, 0, N - 1);
        auto mergeEnd = high_resolution_clock::now();

        cout << "GPU local sort time: "
             << duration<double, milli>(gpuEnd - gpuStart).count() << " ms\n";
        cout << "CPU merge time: "
             << duration<double, milli>(mergeEnd - mergeStart).count() << " ms\n";

        cudaFree(d_data);
        delete[] h_data;
    }

    return 0;
}
