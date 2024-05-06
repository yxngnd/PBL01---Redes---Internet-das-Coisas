#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <unistd.h>
#include <cstring>
#include <nlohmann/json.hpp>
#include "device.hpp"

#define TCP_PORT 54321
#define UDP_PORT 12345
#define MAX_BUFFER_SIZE 1024

using json = nlohmann::json;

void deviceUpdate(LightBulb *device, json data){

    int command = data["command"].get<int>();
    int value = data["value"].get<int>();

    switch(command){
        case 0: {
            device->setId(value);
            break;
        }
        case 1: {
            device->setOn(value);
            break;
        }
        case 2: {
            device->setIntensity(value);
            break;
        }
        case 3: {
            Color newColor = static_cast<Color>(value);
            device->setColor(newColor);
            break;
        }
        
        default:
            break;
    }

}

// Função para enviar dados via UDP
void* sendUDP(void* device_ptr) {
    int sockfd;
    struct sockaddr_in serverAddr;

    LightBulb *device = static_cast<LightBulb*>(device_ptr);

    // Criação do socket
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        perror("Erro ao abrir socket UDP para envio");
        pthread_exit(NULL);
    }

    // Configuração do endereço do servidor UDP
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(UDP_PORT);
    serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");

    while (true) {
        // Criando e populando um objeto JSON
        json data;
        data["id"] = device->getId();
        data["on"] = device->getOn();
        data["intensity"] = device->getIntensity();
        data["color"] = device->getColor();
        // Convertendo o objeto JSON para uma string
        std::string jsonStr = data.dump();

        // Enviando dados via UDP
        if (sendto(sockfd, jsonStr.c_str(), jsonStr.length(), 0, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {
            perror("Erro ao enviar dados via UDP");
            continue; // Continua a solicitar entrada do usuário mesmo se o envio falhar
        }

        sleep(2); // Espera 2 segundos antes de enviar o próximo JSON
    }

    // Fechar socket e finalizar thread
    close(sockfd);
    pthread_exit(NULL);
}

// Função para receber dados via TCP
void* receiveTCP(void* device_ptr) {
    std::cout << "aaaa" << std::endl;
    char buffer[MAX_BUFFER_SIZE];
    int newsockfd;

    LightBulb *device = static_cast<LightBulb*>(device_ptr);

    while (true) {
        int sockfd = socket(AF_INET, SOCK_STREAM, 0);
        if (sockfd < 0) {
            perror("Erro ao abrir socket TCP para envio");
            continue;
        }

        struct sockaddr_in serverAddr;
        serverAddr.sin_family = AF_INET;
        serverAddr.sin_port = htons(TCP_PORT);
        serverAddr.sin_addr.s_addr = inet_addr("0.0.0.0");

        // Loop para tentar conectar até que a conexão seja estabelecida
        while (connect(sockfd, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {
            perror("Erro ao conectar ao servidor TCP");
            sleep(3); // Espera 3 segundos antes de tentar novamente
        }

        while (true) {
            int recvlen = recv(sockfd, buffer, MAX_BUFFER_SIZE, 0);
            if (recvlen > 0) {
                buffer[recvlen] = '\0';

                // Convertendo a string recebida para um objeto JSON
                json receivedData = json::parse(buffer);

                if(!(receivedData.empty())){
                    deviceUpdate(device, receivedData);
                }
                // Imprimindo o JSON recebido
                std::cout << "JSON recebido: " << receivedData << std::endl;
            } 
            else if (recvlen == 0) {
                break;
            } 
            else {
                perror("Erro ao receber dados via TCP");
            }
        }
    }
    
    pthread_exit(NULL);
}
