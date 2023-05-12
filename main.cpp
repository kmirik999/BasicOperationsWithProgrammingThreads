#include <iostream>
#include <chrono>
#include <random>
#include <vector>
#include <thread>

using namespace std;
using namespace chrono;

void matrixVectorMult(int **matrix, int *vectorArray, int *outArray, int n, int start, int end) {
    for (int i = start; i < end; i++) {
        int sum = 0;
        for (int j = 0; j < n; j++) {
            sum += matrix[i][j] * vectorArray[j];
        }
        outArray[i] = sum;
    }
}

int main() {
    int n_values[] = { 50000 };
    const int THREAD_COUNT = 6;
    //const int THREAD_COUNT = thread::hardware_concurrency() / 2;
    //const int THREAD_COUNT = thread::hardware_concurrency();
    //const int THREAD_COUNT = thread::hardware_concurrency() * 2;
    //const int THREAD_COUNT = thread::hardware_concurrency() * 4;
    //const int THREAD_COUNT = thread::hardware_concurrency() * 8;
    //const int THREAD_COUNT = thread::hardware_concurrency() * 16;
    for (int n : n_values) {
        int **matrix = new int *[n];
        int *vectorArray = new int[n];
        int *outArray = new int[n];
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

        //cout << "Matrix:" << endl;
        //for (int i = 0; i < n; i++) {
        //    for (int j = 0; j < n; j++) {
        //        cout << matrix[i][j] << " ";
        //    }
        //    cout << endl;
        //}

        //cout << "Vector:" << endl;
        //for (int i = 0; i < n; i++) {
        //    cout << vectorArray[i] << " ";
        //}
        //cout << endl;

        vector<thread> threads;
        int step = n / THREAD_COUNT;
        auto start = high_resolution_clock::now();
        for (int i = 0; i < THREAD_COUNT; i++) {
            int start = i * step;
            int end = (i == THREAD_COUNT - 1) ? n : start + step;
            threads.emplace_back(matrixVectorMult, matrix, vectorArray, outArray, n, start, end);
        }

        for (auto &thread : threads) {
            thread.join();
        }
        auto stop = high_resolution_clock::now();
        //cout << "Result:" << endl;
        //for (int i = 0; i < n; i++) {
        //    cout << outArray[i] << " ";
        //}
        //cout << endl;


        //duration<double> elapsed = stop - start;


        auto elapsed = duration_cast<nanoseconds>(stop - start);
        printf("Time: %.3f seconds.\n", elapsed.count() * 1e-9);

        double sequential_time = elapsed.count();
        double speedup = sequential_time / elapsed.count();
        double acceleration = speedup / THREAD_COUNT;
        double efficiency = speedup / THREAD_COUNT;

        cout << "Matrix size: " << n << ", Threads: " << THREAD_COUNT << "\n";
        cout << "Speedup: " << speedup << endl;
        cout << "Coefficient of acceleration: " << acceleration << endl;
        cout << "Efficiency: " << efficiency << endl;

        for (int i = 0; i < n; i++) {
            delete[] matrix[i];
        }
        delete[] matrix;
        delete[] vectorArray;
        delete[] outArray;
    }

    return 0;
}