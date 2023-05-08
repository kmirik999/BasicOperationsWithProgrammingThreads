#include <iostream>
#include <chrono>
#include <random>
#include <vector>
#include <thread>

using namespace std;
using namespace chrono;

void matrixVectorMult(int **matrix, int *vectorArray, int n, int start, int end) {
    for (int i = start; i < end; i++) {
        int sum = 0;
        for (int j = 0; j < n; j++) {
            sum += matrix[i][j] * vectorArray[j];
        }
        vectorArray[i] = sum;
    }
}

int main() {
    int n_values[] = {100000, 500000, 1000000, 15000000,2000000};
    //const int THREAD_COUNT = 6;
    //const int THREAD_COUNT = thread::hardware_concurrency() / 2;
    const int THREAD_COUNT = thread::hardware_concurrency();
    //const int THREAD_COUNT = thread::hardware_concurrency() * 2;
    //const int THREAD_COUNT = thread::hardware_concurrency() * 4;
    //const int THREAD_COUNT = thread::hardware_concurrency() * 8;
    //const int THREAD_COUNT = thread::hardware_concurrency() * 16;
    for (int n: n_values) {
        int **matrix = new int *[n];
        int *vectorArray = new int[n];
        for (int i = 0; i < n; i++) {
            matrix[i] = new int[n];
        }

        random_device rd;
        mt19937 gen(rd());
        uniform_int_distribution<> dis(0, 9);
        for (int i = 0; i < n; i++) {
            vectorArray[i] = dis(gen);
            for (int j = 0; j < n; j++) {
                matrix[i][j] = dis(gen);
            }
        }

        cout << "Matrix:" << endl;
        for (int i = 0; i < n; i++) {
            for (int j = 0; j < n; j++) {
                cout << matrix[i][j] << " ";
            }
            cout << endl;
        }

        cout << "Vector:" << endl;
        for (int i = 0; i < n; i++) {
            cout << vectorArray[i] << " ";
        }
        cout << endl;

        vector<thread> threads;
        int step = n / THREAD_COUNT;
        for (int i = 0; i < THREAD_COUNT; i++) {
            int start = i * step;
            int end = (i == THREAD_COUNT - 1) ? n : (i + 1) * step;
            threads.emplace_back(matrixVectorMult, matrix, vectorArray, n, start, end);
        }

        auto start = high_resolution_clock::now();

        for (auto &thread: threads) {
            thread.join();
        }
        cout << "Result:" << endl;
        for (int i = 0; i < n; i++) {
            cout << vectorArray[i] << " ";
        }
        cout << endl;

        auto stop = high_resolution_clock::now();
        auto duration = duration_cast<milliseconds>(stop - start);
        cout << "Execution time: " << duration.count() << " milliseconds" << endl;

        for (int i = 0; i < n; i++) {
            delete[] matrix[i];
        }
        delete[] matrix;
        delete[] vectorArray;

        return 0;
    }
}