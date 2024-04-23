#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <unistd.h>
#include <cstring>
#include <nlohmann/json.hpp>
#include <boost/asio.hpp>
#include <boost/beast.hpp>


#define TCP_PORT 54321
#define UDP_PORT 12345
#define MAX_BUFFER_SIZE 1024

using json = nlohmann::json;
using tcp = boost::asio::ip::tcp;
namespace http = boost::beast::http;

void handleRequest(http::request<http::string_body>& request, tcp::socket& socket) {
    // Prepare the response message
    http::response<http::string_body> response;
    response.version(request.version());
    response.result(http::status::ok);
    response.set(http::field::server, "My HTTP Server");
    response.set(http::field::content_type, "text/plain");
    response.body() = "Hello, World!";
    response.prepare_payload();

    // Send the response to the client
    boost::beast::http::write(socket, response);
}

void runServer() {
    boost::asio::io_context io_context;
    tcp::acceptor acceptor(io_context, {tcp::v4(), 8080});

    while (true) {
        tcp::socket socket(io_context);
        acceptor.accept(socket);

        // Read the HTTP request
        boost::beast::flat_buffer buffer;
        http::request<http::string_body> request;
        boost::beast::http::read(socket, buffer, request);

        // Handle the request
        handleRequest(request, socket);

        // Close the socket
        socket.shutdown(tcp::socket::shutdown_send);
    }
}

// Função para enviar dados via TCP
void* sendTCP(void* arg) {
    int sockfd;
    struct sockaddr_in serverAddr;

    // Criação do socket
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("Erro ao abrir socket TCP para envio");
        pthread_exit(NULL);
    }

    // Configuração do endereço do servidor TCP
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(TCP_PORT);
    serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");

    // Conexão ao servidor TCP
    if (connect(sockfd, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {
        perror("Erro ao conectar ao servidor TCP");
        close(sockfd);
        pthread_exit(NULL);
    }

    while (true) {
        // Criando e populando um objeto JSON
        json data;
        data["nome"] = "João";
        data["idade"] = 30;

        // Convertendo o objeto JSON para uma string
        std::string jsonStr = data.dump();

        // Enviando dados via TCP
        if (send(sockfd, jsonStr.c_str(), jsonStr.length(), 0) < 0) {
            perror("Erro ao enviar dados via TCP");
            break; // Encerra o loop se houver erro no envio
        }

        sleep(2); // Espera 2 segundos antes de enviar o próximo JSON
    }

    // Fechar socket e finalizar thread
    close(sockfd);
    pthread_exit(NULL);
}

// Função para receber dados via UDP
void* receiveUDP(void* arg) {
    int sockfd;
    struct sockaddr_in serverAddr, clientAddr;
    socklen_t clientLen;
    char buffer[MAX_BUFFER_SIZE];

    // Criação do socket
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        perror("Erro ao abrir socket UDP para recebimento");
        pthread_exit(NULL);
    }

    // Configuração do endereço do servidor UDP
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(UDP_PORT);
    serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);

    // Associação do socket ao endereço do servidor UDP
    if (bind(sockfd, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {
        perror("Erro ao associar socket ao endereço UDP");
        close(sockfd);
        pthread_exit(NULL);
    }

    // Recebimento de dados
    while (true) {
        clientLen = sizeof(clientAddr);
        int recvlen = recvfrom(sockfd, buffer, MAX_BUFFER_SIZE, 0, (struct sockaddr*)&clientAddr, &clientLen);
        if (recvlen > 0) {
            buffer[recvlen] = '\0';

            // Convertendo a string recebida para um objeto JSON
            json receivedData = json::parse(buffer);

            // Imprimindo o JSON recebido
            std::cout << "JSON recebido via UDP: " << receivedData << std::endl;
        } else if (recvlen == 0) {
            std::cerr << "Conexão fechada pelo cliente" << std::endl;
            break;
        } else {
            perror("Erro ao receber dados via UDP");
        }
    }

    // Fechar socket e finalizar thread
    close(sockfd);
    pthread_exit(NULL);
}

int main() {
    pthread_t sendThread, receiveThread;

    try {
        runServer();
    } catch (const std::exception& e) {
        std::cerr << "Exception: " << e.what() << std::endl;
    }

    // Criando threads para envio e recebimento
    if (pthread_create(&sendThread, NULL, sendTCP, NULL) != 0) {
        std::cerr << "Erro ao criar thread de envio via TCP" << std::endl;
        return 1;
    }
    if (pthread_create(&receiveThread, NULL, receiveUDP, NULL) != 0) {
        std::cerr << "Erro ao criar thread de recebimento via UDP" << std::endl;
        return 1;
    }

    // Esperar pelo término das threads
    pthread_join(sendThread, NULL);
    pthread_join(receiveThread, NULL);

    return 0;
}
