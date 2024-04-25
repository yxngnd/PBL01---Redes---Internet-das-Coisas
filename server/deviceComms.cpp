#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <unistd.h>
#include <cstring>
#include <nlohmann/json.hpp>
#include "device.hpp"

#define SERVER_IP "127.0.0.1"
#define TCP_PORT 54321
#define UDP_PORT 12345
#define MAX_BUFFER_SIZE 1024

using json = nlohmann::json;



void* sendUDP(void* arg) {
    int sockfd;
    struct sockaddr_in serverAddr;

    LightBulb light(14700, true, 0, Color::WHITE);

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        perror("Erro ao abrir socket UDP para envio");
        pthread_exit(NULL);
    }

    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(UDP_PORT);
    serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");

    while (true) {
        json data;
        data["on"] = light.getOn();
        data["intensity"] = light.getIntensity();
        data["color"] = light.getColor();
        std::string jsonStr = data.dump();

        if (sendto(sockfd, jsonStr.c_str(), jsonStr.length(), 0, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {
            perror("Erro ao enviar dados via UDP");
            continue;
        }

        sleep(2);
    }

    close(sockfd);
    pthread_exit(NULL);
}

void* receiveTCP(void* arg) {
    int sockfd, newsockfd;
    struct sockaddr_in serverAddr, clientAddr;
    socklen_t clientLen;
    char buffer[MAX_BUFFER_SIZE];

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("Erro ao abrir socket TCP para recebimento");
        pthread_exit(NULL);
    }

    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(TCP_PORT);
    serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);

    if (bind(sockfd, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {
        perror("Erro ao associar socket ao endereço TCP");
        close(sockfd);
        pthread_exit(NULL);
    }

    if (listen(sockfd, 5) < 0) {
        perror("Erro ao escutar por conexões TCP");
        close(sockfd);
        pthread_exit(NULL);
    }

    clientLen = sizeof(clientAddr);
    newsockfd = accept(sockfd, (struct sockaddr*)&clientAddr, &clientLen);
    if (newsockfd < 0) {
        perror("Erro ao aceitar conexão TCP");
        close(sockfd);
        pthread_exit(NULL);
    }

    while (true) {
        int recvlen = recv(newsockfd, buffer, MAX_BUFFER_SIZE, 0);
        if (recvlen > 0) {
            buffer[recvlen] = '\0';

            json receivedData = json::parse(buffer);

            std::cout << "JSON recebido: " << receivedData << std::endl;
        } else if (recvlen == 0) {
            std::cerr << "Conexão fechada pelo cliente" << std::endl;
            break;
        } else {
            perror("Erro ao receber dados via TCP");
        }
    }

    close(newsockfd);
    close(sockfd);
    pthread_exit(NULL);
}

int main(){

    pthread_t sendThread, receiveThread;

    if (pthread_create(&sendThread, NULL, sendUDP, NULL) != 0) {
        std::cerr << "Erro ao criar thread de envio" << std::endl;
        return 1;
    }
    if (pthread_create(&receiveThread, NULL, receiveTCP, NULL) != 0) {
        std::cerr << "Erro ao criar thread de recebimento" << std::endl;
        return 1;
    }

    pthread_join(sendThread, NULL);
    pthread_join(receiveThread, NULL);


    return 0;
}

