#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <strings.h>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>
#include "device.hpp"

#define SERVER_IP "127.0.0.1"
#define TCP_PORT 54321
#define UDP_PORT 12345
#define MAX_LINE 1024

int main(){

    int tcpSocket;
    struct sockaddr_in tcpServerAddr;

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

    LightBulb light(true, 0, Color::WHITE);
    while (true) {
        // Simulando leitura de dados do sensor
        const char *tcpMessage = light.getColor().c_str();

        // Enviando dados via UDP para o servidor
        send(tcpSocket, tcpMessage, strlen(tcpMessage), 0);

        // Aguardando um tempo antes de enviar o próximo conjunto de dados
        sleep(1);
    }

    close(tcpSocket);

    return 0;
}

