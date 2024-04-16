#include <sys/socket.h>
#include <iostream>
#include <netinet/in.h>
#include <strings.h>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>

#define TCP_PORT 54321
#define UDP_PORT 12345
#define MAX_LINE 1024

int main(){

    struct sockaddr_in udpServerAddr, tcpServerAddr, tcpClientAddr;
    char buffer[MAX_LINE];
    socklen_t addrSize = sizeof(tcpClientAddr);
    int udpSocket, tcpSocket, tcpClientSocket;

    memset(&udpServerAddr, 0, sizeof(udpServerAddr));
    udpServerAddr.sin_family = AF_INET;
    udpServerAddr.sin_addr.s_addr = INADDR_ANY;
    //udpServerAddr.sin_addr.s_addr = inet_addr(""); // Endereço IP específico
    udpServerAddr.sin_port = htons(UDP_PORT);

    if ((udpSocket = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        std::perror("Error creating UDP socket.");
        std::exit(1);
    }

    if (bind(udpSocket, (const struct sockaddr *)&udpServerAddr, sizeof(udpServerAddr)) < 0) {
        std::perror("Error connecting UDP socket.");
        std::exit(1);
    }

    const char *udpMessage = "Message via UDP.";
    sendto(udpSocket, udpMessage, strlen(udpMessage), 0, (const struct sockaddr *)&udpServerAddr, sizeof(udpServerAddr));
    std::cout << "Data sent via UDP." << std::endl;

    if ((tcpSocket = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        std::perror("Error creating TCP socket.");
        std::exit(1);
    }

    memset(&tcpServerAddr, 0, sizeof(tcpServerAddr));
    tcpServerAddr.sin_family = AF_INET;
    tcpServerAddr.sin_addr.s_addr = INADDR_ANY;
    //tcpServerAddr.sin_addr.s_addr = inet_addr(""); // Endereço IP específico
    tcpServerAddr.sin_port = htons(TCP_PORT);

    // Ligação do socket TCP
    if (bind(tcpSocket, (const struct sockaddr *)&tcpServerAddr, sizeof(tcpServerAddr)) < 0) {
        std::perror("Error connecting TCP socket.");
        std::exit(1);
    }

     // Espera por conexão TCP
    if (listen(tcpSocket, 1) < 0) {
        std::perror("Error listening to TCP connections.");
        std::exit(1);
    }

    if ((tcpClientSocket = accept(tcpSocket, (struct sockaddr *)&tcpClientAddr, &addrSize)) < 0) {
        perror("Error accepting TCP connection.");
        std::exit(1);
    }

    // Recebimento de dados via TCP
    ssize_t bytesReceived = recv(tcpClientSocket, buffer, MAX_LINE, 0);
    if (bytesReceived < 0) {
        std::perror("Error receiving TCP data.");
        std::exit(1);
    }

    buffer[bytesReceived] = '\0';
    std::cout << "Data received via TCP: " << buffer << std::endl;

    close(udpSocket);
    close(tcpSocket);
    close(tcpClientSocket);

    return 0;
}