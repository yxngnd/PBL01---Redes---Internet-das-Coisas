#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <strings.h>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>

#define TCP_PORT 54321
#define UDP_PORT 12345
#define MAX_LINE 1024

//int argc, char *argv[]
int main(){


    int tcpSocket, udpSocket;
    struct sockaddr_in tcpServerAddr, udpServerAddr;
    char buffer[MAX_LINE];

    if ((tcpSocket = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        std::perror("Error creating TCP socket.");
        std::exit(1);
    }

    memset(&tcpServerAddr, 0, sizeof(tcpServerAddr));
    tcpServerAddr.sin_family = AF_INET;
    tcpServerAddr.sin_port = htons(TCP_PORT);

    if (connect(tcpSocket, (const struct sockaddr *)&tcpServerAddr, sizeof(tcpServerAddr)) < 0) {
        std::perror("Error connecting TCP socket.");
        std::exit(1);
    }

    const char *tcpMessage = "Message via TCP";
    send(tcpSocket, tcpMessage, strlen(tcpMessage), 0);
    std::cout << "Data sent via TCP." << std::endl;

    close(tcpSocket);

    if ((udpSocket = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        std::perror("Error creating UDP socket.");
        std::exit(1);
    }

    memset(&udpServerAddr, 0, sizeof(udpServerAddr));
    udpServerAddr.sin_family = AF_INET;
    udpServerAddr.sin_port = htons(UDP_PORT);
    udpServerAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    //udpServerAddr.sin_addr.s_addr = inet_addr(""); // Endereço IP específico

    if (bind(udpSocket, (const struct sockaddr *)&udpServerAddr, sizeof(udpServerAddr)) < 0) {
        perror("Error connecting UDP socket.");
        std::exit(1);
    }

    socklen_t addrSize = sizeof(udpServerAddr);
    ssize_t bytesReceived = recvfrom(udpSocket, buffer, MAX_LINE, 0, (struct sockaddr *)&udpServerAddr, &addrSize);
    if (bytesReceived < 0) {
        perror("Error receiving UDP data.");
        std::exit(1);
    }

    buffer[bytesReceived] = '\0';
    std::cout << "Data received via UDP: " << buffer << std::endl;

    close(udpSocket);
    
    return 0;
}