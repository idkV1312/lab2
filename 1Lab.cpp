#include <cstdlib>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <chrono>
#include <string>
#include <vector>
#include <windows.h>

using namespace std;
using namespace chrono;

// Функция записи матрицы в файл
void writeMatrixToFile(const vector<float>& mat, int size, const string& filename)
{
    ofstream fout(filename);
    if (!fout)
    {
        cerr << "Не удалось открыть файл " << filename << endl;
        return;
    }

    fout << fixed << setprecision(10);

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

    vector<int> sizes = { 200, 400, 800, 1200, 1600, 2000, 2400, 2800, 3200, 3600 };

    ofstream timeFile("timings.txt");
    if (!timeFile)
    {
        cerr << "Не удалось создать файл timings.txt" << endl;
        return -1;
    }

    timeFile << "N\ttime(s)\n";

    for (int N : sizes)
    {
        cout << "Обработка N = " << N << " ..." << endl;

        vector<float> A(N * N);
        vector<float> B(N * N);
        vector<float> C(N * N, 0.0f);

        // Инициализация
        for (int i = 0; i < N; ++i)
            for (int j = 0; j < N; ++j)
            {
                int idx = i * N + j;
                A[idx] = (i + 1) * (j + 1);
                B[idx] = (i + 1) + 2 * (j + 1);
            }

        // Сохранение исходных матриц
        writeMatrixToFile(A, N, "A_" + to_string(N) + ".txt");
        writeMatrixToFile(B, N, "B_" + to_string(N) + ".txt");

        auto start = high_resolution_clock::now();

        // Перемножение
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

        // Сохранение результата
        writeMatrixToFile(C, N, "C_" + to_string(N) + ".txt");

        timeFile << N << "\t" << seconds << "\n";

        cout << "Готово N = " << N << ", время = " << seconds << " с\n";
    }

    cout << "Все измерения завершены. Результаты сохранены в timings.txt\n";
    return 0;
}