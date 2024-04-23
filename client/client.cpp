#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <unistd.h>
#include <cstring>
#include <nlohmann/json.hpp>

#define TCP_PORT 54321
#define UDP_PORT 12345
#define MAX_BUFFER_SIZE 1024

using json = nlohmann::json;

// Função para enviar dados via UDP
void* sendUDP(void* arg) {
    int sockfd;
    struct sockaddr_in serverAddr;

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
        data["nome"] = "João";
        data["idade"] = 30;

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
void* receiveTCP(void* arg) {
    int sockfd, newsockfd;
    struct sockaddr_in serverAddr, clientAddr;
    socklen_t clientLen;
    char buffer[MAX_BUFFER_SIZE];

    // Criação do socket
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("Erro ao abrir socket TCP para recebimento");
        pthread_exit(NULL);
    }

    // Configuração do endereço do servidor TCP
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(TCP_PORT);
    serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);

    // Associação do socket ao endereço do servidor TCP
    if (bind(sockfd, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {
        perror("Erro ao associar socket ao endereço TCP");
        close(sockfd);
        pthread_exit(NULL);
    }

    // Escutar conexões
    if (listen(sockfd, 5) < 0) {
        perror("Erro ao escutar por conexões TCP");
        close(sockfd);
        pthread_exit(NULL);
    }

    // Aceitar conexões
    clientLen = sizeof(clientAddr);
    newsockfd = accept(sockfd, (struct sockaddr*)&clientAddr, &clientLen);
    if (newsockfd < 0) {
        perror("Erro ao aceitar conexão TCP");
        close(sockfd);
        pthread_exit(NULL);
    }

    // Recebimento de dados
    while (true) {
        int recvlen = recv(newsockfd, buffer, MAX_BUFFER_SIZE, 0);
        if (recvlen > 0) {
            buffer[recvlen] = '\0';
            std::cout << "Mensagem recebida via TCP: " << buffer << std::endl;

            // Convertendo a string recebida para um objeto JSON
            json receivedData = json::parse(buffer);

            // Imprimindo o JSON recebido
            std::cout << "JSON recebido: " << receivedData << std::endl;
        } else if (recvlen == 0) {
            std::cerr << "Conexão fechada pelo cliente" << std::endl;
            break;
        } else {
            perror("Erro ao receber dados via TCP");
        }
    }

    // Fechar sockets e finalizar thread
    close(newsockfd);
    close(sockfd);
    pthread_exit(NULL);
}

int main() {
    pthread_t sendThread, receiveThread;

    // Criando threads para envio e recebimento
    if (pthread_create(&sendThread, NULL, sendUDP, NULL) != 0) {
        std::cerr << "Erro ao criar thread de envio" << std::endl;
        return 1;
    }
    if (pthread_create(&receiveThread, NULL, receiveTCP, NULL) != 0) {
        std::cerr << "Erro ao criar thread de recebimento" << std::endl;
        return 1;
    }

    // Esperar pelo término das threads
    pthread_join(sendThread, NULL);
    pthread_join(receiveThread, NULL);

    return 0;
}
