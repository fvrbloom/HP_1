#include <iostream>
#include <chrono>
#include <cstdlib>
#include <ctime>
#include <omp.h>

using HighResClock = std::chrono::high_resolution_clock;

/* Проверка доступности OpenMP и количества потоков */
void print_omp_info() {
    std::cout << "OpenMP max threads: " << omp_get_max_threads() << std::endl;

    #pragma omp parallel
    {
        #pragma omp single
        {
            std::cout << "Threads in parallel region: "
                      << omp_get_num_threads() << "\n\n";
        }
    }
}

/* Задание 1: последовательное вычисление среднего значения */
double calc_average_seq(int* data, size_t length) {
    auto t_start = HighResClock::now();

    long long total = 0;
    for (size_t i = 0; i < length; ++i)
        total += data[i];

    double mean = static_cast<double>(total) / length;

    auto t_end = HighResClock::now();
    double elapsed =
        std::chrono::duration<double, std::milli>(t_end - t_start).count();

    std::cout << "[Task 1] Average value: " << mean
              << " | Time: " << elapsed << " ms\n\n";

    return mean;
}

/* Задание 2: последовательный поиск минимума и максимума */
void find_minmax_seq(int* data, size_t length, int& min_res, int& max_res) {
    auto t_start = HighResClock::now();

    min_res = data[0];
    max_res = data[0];

    for (size_t i = 1; i < length; ++i) {
        if (data[i] < min_res) min_res = data[i];
        if (data[i] > max_res) max_res = data[i];
    }

    auto t_end = HighResClock::now();
    double elapsed =
        std::chrono::duration<double, std::milli>(t_end - t_start).count();

    std::cout << "[Task 2] Sequential Min = " << min_res
              << ", Max = " << max_res
              << " | Time: " << elapsed << " ms\n\n";
}

/* Задание 3: параллельный поиск минимума и максимума */
void find_minmax_parallel(int* data, size_t length, int& min_res, int& max_res) {
    auto t_start = HighResClock::now();

    int min_local = data[0];
    int max_local = data[0];

    #pragma omp parallel for reduction(min:min_local) reduction(max:max_local)
    for (size_t i = 0; i < length; ++i) {
        if (data[i] < min_local) min_local = data[i];
        if (data[i] > max_local) max_local = data[i];
    }

    min_res = min_local;
    max_res = max_local;

    auto t_end = HighResClock::now();
    double elapsed =
        std::chrono::duration<double, std::milli>(t_end - t_start).count();

    std::cout << "[Task 3] Parallel Min = " << min_res
              << ", Max = " << max_res
              << " | Time: " << elapsed << " ms\n\n";
}

/* Задание 4: сравнение среднего значения (последовательно / OpenMP) */
void compare_average_methods(int* data, size_t length) {
    // Последовательный вариант
    auto t_start = HighResClock::now();
    long long sum_seq = 0;

    for (size_t i = 0; i < length; ++i)
        sum_seq += data[i];

    double avg_seq = static_cast<double>(sum_seq) / length;
    auto t_end = HighResClock::now();

    double time_seq =
        std::chrono::duration<double, std::milli>(t_end - t_start).count();

    // Параллельный вариант
    t_start = HighResClock::now();
    long long sum_par = 0;

    #pragma omp parallel for reduction(+:sum_par)
    for (size_t i = 0; i < length; ++i)
        sum_par += data[i];

    double avg_par = static_cast<double>(sum_par) / length;
    t_end = HighResClock::now();

    double time_par =
        std::chrono::duration<double, std::milli>(t_end - t_start).count();

    std::cout << "[Task 4] Sequential Avg = " << avg_seq
              << " | Time: " << time_seq << " ms\n";
    std::cout << "[Task 4] Parallel Avg   = " << avg_par
              << " | Time: " << time_par << " ms\n";
}

int main() {
    std::cout << "Program started\n";

    print_omp_info();

    std::srand(static_cast<unsigned>(std::time(nullptr)));

    /* Task 1 */
    const size_t N1 = 50000;
    int* buffer1 = new int[N1];
    for (size_t i = 0; i < N1; ++i)
        buffer1[i] = std::rand() % 100 + 1;

    calc_average_seq(buffer1, N1);
    delete[] buffer1;

    /* Task 2 & 3 */
    const size_t N2 = 1'000'000;
    int* buffer2 = new int[N2];
    for (size_t i = 0; i < N2; ++i)
        buffer2[i] = std::rand() % 100000;

    int min_value, max_value;
    find_minmax_seq(buffer2, N2, min_value, max_value);
    find_minmax_parallel(buffer2, N2, min_value, max_value);

    delete[] buffer2;

    /* Task 4 */
    const size_t N4 = 5'000'000;
    int* buffer4 = new int[N4];
    for (size_t i = 0; i < N4; ++i)
        buffer4[i] = std::rand() % 100000;

    compare_average_methods(buffer4, N4);
    delete[] buffer4;

    return 0;
}
