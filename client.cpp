#include <iostream>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <csignal>
#include <random>

using namespace std;

void handleSIGPIPE(int signal) {
    // Handle the SIGPIPE signal
}

int main() {
    // Ignore the SIGPIPE signal
    signal(SIGPIPE, handleSIGPIPE);
    int clientSocket;
    struct sockaddr_in serverAddr{};
    const char* SERVER_IP = "127.0.0.1";
    const int PORT = 8081;

    // Create client socket
    clientSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (clientSocket == -1) {
        perror("Failed to create socket");
        exit(EXIT_FAILURE);
    }

    //  address
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(PORT);
    if (inet_pton(AF_INET, SERVER_IP, &(serverAddr.sin_addr)) <= 0) {
        perror("Invalid address/Address not supported");
        exit(EXIT_FAILURE);
    }

    // Connect to the server
    if (connect(clientSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == -1) {
        perror("Connection failed");
        exit(EXIT_FAILURE);
    }

    int n = 5000;
    if (send(clientSocket, (char*)&n, sizeof(n), 0) == -1) {
        perror("Send failed");
        exit(EXIT_FAILURE);
    }

    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> dis(1, 9);

    int **matrix = new int*[n];
    int *vectorArray = new int[n];
    int *outArray = new int[n];

    for (int i = 0; i < n; i++) {
        matrix[i] = new int[n];
        for (int j = 0; j < n; j++) {
            matrix[i][j] = dis(gen);
        }
        if (send(clientSocket, (char*)matrix[i], n * sizeof(int), 0) == -1) {
            perror("Send failed");
            exit(EXIT_FAILURE);
        }
    }

    uniform_int_distribution<> vectorDis(1, 9);
    for (int i = 0; i < n; i++) {
        vectorArray[i] = vectorDis(gen);
    }


    cout << "Matrix sent to server:" << endl;
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            cout << matrix[i][j] << " ";
        }
        cout << endl;
    }
    cout << "Vector sent to server:" << endl;
    for (int i = 0; i < n; i++) {
        cout << vectorArray[i] << " ";
    }
    cout << endl;

    if (send(clientSocket, (char*)vectorArray, n * sizeof(int), 0) == -1) {
        perror("Send failed");
        exit(EXIT_FAILURE);
    }

    char command = 'C';
    if (send(clientSocket, &command, sizeof(command), 0) == -1) {
        perror("Send failed");
        exit(EXIT_FAILURE);
    }

    cout << "Command sent to server." << endl;

    int receivedBytes = recv(clientSocket, (char*)outArray, n * sizeof(int), 0);
    if (receivedBytes == -1) {
        perror("Receive failed");
        exit(EXIT_FAILURE);
    } else if (receivedBytes == 0) {
        cout << "Connection closed by the server." << endl;
    } else {
        cout << "Result received from server:" << endl;
        for (int i = 0; i < n; i++) {
            cout << outArray[i] << " ";
        }
        cout << endl;
    }

    for (int i = 0; i < n; i++) {
        delete[] matrix[i];
    }
    delete[] matrix;
    delete[] vectorArray;
    delete[] outArray;

    close(clientSocket);

    return 0;
}
