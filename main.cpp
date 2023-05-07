#include <iostream>
#include <chrono>
#include <random>

using namespace std;
using namespace chrono;

int main() {
    const int n = 10000;
    int **matrix = new int*[n];
    int *vector = new int[n];
    for (int i = 0; i < n; i++) {
        matrix[i] = new int[n];
    }

    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> dis(0, 9);
    for (int i = 0; i < n; i++) {
        vector[i] = dis(gen);
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
        cout << vector[i] << " ";
    }
    cout << endl;

    auto start = high_resolution_clock::now();

    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            vector[i] += matrix[i][j];
        }
    }

    auto stop = high_resolution_clock::now();
    auto duration = duration_cast<milliseconds>(stop - start);
    cout << "Execution time: " << duration.count() << " milliseconds" << endl;

    for (int i = 0; i < n; i++) {
        delete[] matrix[i];
    }
    delete[] matrix;
    delete[] vector;

    return 0;
}
