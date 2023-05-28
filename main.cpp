#include <iostream>
#include <chrono>
#include <vector>
#include <thread>
#include <cstring>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

using namespace std;
using namespace chrono;

void matrixVectorMult(int **matrix, const int *vectorArray, int *outArray, int n, int start, int end) {
    for (int i = start; i < end; i++) {
        int sum = 0;
        for (int j = 0; j < n; j++) {
            sum += matrix[i][j] * vectorArray[j];
        }
        outArray[i] = sum;
    }
}

int main() {
    int serverSocket, clientSocket;
    struct sockaddr_in serverAddr{}, clientAddr{};
    socklen_t clientAddrLen;
    const int PORT = 8081;
    const int BACKLOG = 5;

    // Create server socket
    serverSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (serverSocket == -1) {
        perror("Failed to create socket");
        exit(EXIT_FAILURE);
    }

    // address
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(PORT);

    // Bind the socket to an address and port
    if (::bind(serverSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == -1) {
        perror("Bind failed");
        exit(EXIT_FAILURE);
    }

    // listening for incoming connections
    if (listen(serverSocket, BACKLOG) == -1) {
        perror("Listen failed");
        exit(EXIT_FAILURE);
    }

    cout << "Server started. Listening on port " << PORT << endl;

    // Accept incoming connection
    clientAddrLen = sizeof(clientAddr);
    clientSocket = accept(serverSocket, (struct sockaddr*)&clientAddr, &clientAddrLen);
    if (clientSocket == -1) {
        perror("Accept failed");
        exit(EXIT_FAILURE);
    }

    cout << "New client connected" << endl;

    int n;
    if (recv(clientSocket, (char*)&n, sizeof(n), 0) == -1) {
        perror("Receive failed");
        exit(EXIT_FAILURE);
    }

    int **matrix = new int*[n];
    int *vectorArray = new int[n];
    int *outArray = new int[n];

    for (int i = 0; i < n; i++) {
        matrix[i] = new int[n];
        if (recv(clientSocket, (char*)matrix[i], n * sizeof(int), 0) == -1) {
            perror("Receive failed");
            exit(EXIT_FAILURE);
        }
    }

    if (recv(clientSocket, (char*)vectorArray, n * sizeof(int), 0) == -1) {
        perror("Receive failed");
        exit(EXIT_FAILURE);
    }

    cout << "Matrix:" << endl;
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            cout << matrix[i][j] << " ";
        }
        cout << endl;
    }

    cout << "Vector received from client:" << endl;
    for (int i = 0; i < n; i++) {
        cout << vectorArray[i] << " ";
    }
    cout << endl;

    char command;
    if (recv(clientSocket, &command, sizeof(command), 0) == -1) {
        perror("Receive failed");
        exit(EXIT_FAILURE);
    }

    if (command == 'C') {
        int THREAD_COUNT = thread::hardware_concurrency();
        vector<thread> threads;
        int step = n / THREAD_COUNT;
        auto start = high_resolution_clock::now();
        for (int i = 0; i < THREAD_COUNT; i++) {
            int start = i * step;
            int end = (i == THREAD_COUNT - 1) ? n : start + step;
            threads.emplace_back(matrixVectorMult, matrix, vectorArray, outArray, n, start, end);
        }

        for (int i = 0; i < THREAD_COUNT; i++) {
            threads[i].join();
        }
        auto stop = high_resolution_clock::now();

        auto elapsed = duration_cast<nanoseconds>(stop - start);
        printf("Time: %.3f seconds.\n", elapsed.count() * 1e-9);
        cout << "Result:" << endl;
        for (int i = 0; i < n; i++) {
            cout << outArray[i] << " ";
        }
        cout << endl;
        if (send(clientSocket, (char*)outArray, n * sizeof(int), MSG_NOSIGNAL) == -1) {
            if (errno == EPIPE) {
                // Handle the "Broken pipe" error here
                cerr << "Send failed: Broken pipe" << endl;
            } else {
                cerr << "Send failed: " << strerror(errno) << endl;
            }
            exit(EXIT_FAILURE);
        }
        cout << "Calculation completed. Result sent to client." << endl;

        for (int i = 0; i < n; i++) {
            delete[] matrix[i];
        }
        delete[] matrix;
        delete[] outArray;
    }
    delete[] vectorArray;
    close(clientSocket);
    close(serverSocket);
    return 0;
}
