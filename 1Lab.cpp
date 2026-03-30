#include <cstdlib>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <chrono>
#include <string>
#include <vector>
#include <windows.h>
#include <omp.h>

using namespace std;
using namespace chrono;

// Запись матрицы в файл
void writeMatrixToFile(const vector<float>& mat, int size, const string& filename)
{
    ofstream fout(filename);
    if (!fout)
    {
        cerr << "Не удалось открыть файл " << filename << endl;
        return;
    }

    fout << fixed << setprecision(6);

    for (int i = 0; i < size; ++i)
    {
        for (int j = 0; j < size; ++j)
        {
            fout << mat[i * size + j];
            if (j != size - 1) fout << " ";
        }
        fout << "\n";
    }
}

int main()
{
    SetConsoleOutputCP(1251);
    SetConsoleCP(1251);

    vector<int> sizes = {200, 400, 800, 1200, 1600, 2000};
    vector<int> threads_list = {1, 2, 4, 8};

    ofstream timeFile("timings_omp.txt");
    timeFile << "Threads\tN\tTime(s)\n";

    for (int num_threads : threads_list)
    {
        omp_set_num_threads(num_threads);
        cout << "\n===== Потоки: " << num_threads << " =====\n";

        for (int N : sizes)
        {
            cout << "Обработка N = " << N << " ..." << endl;

            vector<float> A(N * N);
            vector<float> B(N * N);
            vector<float> C(N * N, 0.0f);

            // Инициализация
#pragma omp parallel for
            for (int i = 0; i < N; ++i)
                for (int j = 0; j < N; ++j)
                {
                    int idx = i * N + j;
                    A[idx] = (i + 1) * (j + 1);
                    B[idx] = (i + 1) + 2 * (j + 1);
                }

            auto start = high_resolution_clock::now();

            // Параллельное умножение матриц
#pragma omp parallel for collapse(2)
            for (int i = 0; i < N; ++i)
                for (int j = 0; j < N; ++j)
                {
                    float sum = 0.0f;
                    for (int k = 0; k < N; ++k)
                        sum += A[i * N + k] * B[k * N + j];

                    C[i * N + j] = sum;
                }

            auto end = high_resolution_clock::now();
            double seconds = duration<double>(end - start).count();

            cout << "Готово N = " << N
                 << ", потоки = " << num_threads
                 << ", время = " << seconds << " с\n";

            timeFile << num_threads << "\t" << N << "\t" << seconds << "\n";

            if (num_threads == 1)
            {
                writeMatrixToFile(A, N, "A_" + to_string(N) + ".txt");
                writeMatrixToFile(B, N, "B_" + to_string(N) + ".txt");
                writeMatrixToFile(C, N, "C_" + to_string(N) + ".txt");
            }
        }
    }

    cout << "\nВсе эксперименты завершены. См. timings_omp.txt\n";
    return 0;
}