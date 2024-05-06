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

void sendTCP(int conn);

using json = nlohmann::json;
using tcp = boost::asio::ip::tcp;
namespace http = boost::beast::http;

int idDevice = 0;

struct Connection {
    int id;
    int sockfd;
    Connection(int _id, int _sockfd) : id(_id), sockfd(_sockfd) {}
};

int sockTCP = socket(AF_INET, SOCK_STREAM, 0);
int sockUDP = socket(AF_INET, SOCK_DGRAM, 0);

std::vector<Connection> conns;
std::vector<json> devices;
json commandData;
int idCommand = 1;

void handlePostRequest(http::request<http::string_body>& request, tcp::socket& socket) {
    try {
        // Verificar se o corpo da requisição está presente
        if (request.body().empty()) {
            throw std::invalid_argument("Empty request body");
        }

        // Parse JSON from request body
        json receivedData = json::parse(request.body());

        // Perform some action with received data
        // Here you can edit data or perform any other action based on the received data
        commandData = receivedData;
        sendTCP(receivedData["id"]);

        // Prepare the JSON response
        json jsonResponse;
        jsonResponse["status"] = "success";
        jsonResponse["message"] = "Data successfully updated";

        // Prepare the response message
        http::response<http::string_body> response;
        response.version(request.version());
        response.result(http::status::ok);
        response.set(http::field::server, "My HTTP Server");
        response.set(http::field::content_type, "application/json");
        response.body() = jsonResponse.dump();
        response.prepare_payload();

        // Send the response to the client
        boost::beast::http::write(socket, response);
    } catch (const std::exception& e) {
        std::cerr << "Error handling POST request: " << e.what() << std::endl;

        // Prepare an error JSON response
        json errorResponse;
        errorResponse["status"] = "error";
        errorResponse["message"] = "Error handling POST request";

        // Prepare the response message
        http::response<http::string_body> response;
        response.version(request.version());
        response.result(http::status::bad_request);
        response.set(http::field::server, "My HTTP Server");
        response.set(http::field::content_type, "application/json");
        response.body() = errorResponse.dump();
        response.prepare_payload();

        // Send the error response to the client
        boost::beast::http::write(socket, response);
    }
}

void handleGetRequest(http::request<http::string_body>& request, tcp::socket& socket) {
    try {
        // Prepare some data to send
        json data;
        data["devices"] = devices;

        // Check if data is empty
        if (data.empty()) {
            throw std::runtime_error("No data available");
        }

        // Convert data to JSON string
        std::string jsonData = data.dump();

        // Prepare the response message
        http::response<http::string_body> response;
        response.version(request.version());
        response.result(http::status::ok);
        response.set(http::field::server, "My HTTP Server");
        response.set(http::field::content_type, "application/json");
        response.body() = jsonData;
        response.prepare_payload();

        // Send the response to the client
        boost::beast::http::write(socket, response);
    } catch (const std::exception& e) {
        std::cerr << "Error handling GET request: " << e.what() << std::endl;

        // Prepare an error JSON response
        json errorResponse;
        errorResponse["status"] = "error";
        errorResponse["message"] = "Error handling POST request";

        // Prepare an error response
        http::response<http::string_body> response;
        response.version(request.version());
        response.result(http::status::internal_server_error);
        response.set(http::field::server, "My HTTP Server");
        response.set(http::field::content_type, "text/plain");
        response.body() = errorResponse.dump();
        response.prepare_payload();

        // Send the error response to the client
        boost::beast::http::write(socket, response);
    }
}

void handleRequest(http::request<http::string_body>& request, tcp::socket& socket) {
    // Check HTTP method
    if (request.method() == http::verb::post) {
        handlePostRequest(request, socket);
    } else if (request.method() == http::verb::get) {
        handleGetRequest(request, socket);
    } else {
        // Unsupported HTTP method
        http::response<http::string_body> response;
        response.version(request.version());
        response.result(http::status::bad_request);
        response.set(http::field::server, "My HTTP Server");
        response.set(http::field::content_type, "text/plain");
        response.body() = "Unsupported HTTP method";
        response.prepare_payload();

        // Send the response to the client
        boost::beast::http::write(socket, response);
    }
}

void* runServer(void* arg) {
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
    pthread_exit(NULL);
}

bool connectionExists(int sockfd) {
    for (Connection conn : conns) {
        if (sockfd == conn.sockfd) {
            return true;
        }
    }
    return false;
}

// Função para aceitar conexões de clientes
void* establishConnections(void* arg) {
    int newsockTCP;
    struct sockaddr_in serverAddr, clientAddr;
    socklen_t clientLen;


    while (true) {
        // Criação do socket
        if (sockTCP < 0) {
            perror("Erro ao abrir socket TCP para recebimento");
            sleep(3); // Tentar novamente após 3 segundos
            continue;
        }
        // Configuração do endereço do servidor TCP
        serverAddr.sin_family = AF_INET;
        serverAddr.sin_port = htons(TCP_PORT);
        serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);
        // Associação do socket ao endereço do servidor TCP
        bind(sockTCP, (struct sockaddr*)&serverAddr, sizeof(serverAddr));
        // Escutar conexões
        if (listen(sockTCP, 5) < 0) {
            perror("Erro ao escutar por conexões TCP");
            close(sockTCP);
            sleep(3); // Tentar novamente após 3 segundos
            continue;
        }
        // Aceitar conexões
        clientLen = sizeof(clientAddr);
        
        newsockTCP = accept(sockTCP, (struct sockaddr*)&clientAddr, &clientLen);
        
        // Recebimento de dados
        if (!connectionExists(newsockTCP)) {
            Connection newConn(idDevice, newsockTCP);
            conns.push_back(newConn);
            commandData["command"] = 0;
            commandData["value"] = idDevice;
            sendTCP(newConn.id);
            commandData = json();
            idDevice++;
            // Fechar sockets e aguardar 3 segundos antes de tentar reconectar
            //sleep(3); // Tentar novamente após 3 segundos
                
        }
    }
    pthread_exit(NULL);
}

void sendTCP(int id) {
    int sendConn;
    for (Connection conn : conns) {
        if (id == conn.id) {
            sendConn = conn.sockfd;
            break;
        }
    }
    if(!(commandData.empty())){
        // Criando e populando um objeto JSON
        json sendData = commandData;

        // Convertendo o objeto JSON para uma string
        std::string jsonStr = sendData.dump();

        // Enviando dados via TCP
        send(sendConn, jsonStr.c_str(), jsonStr.length(), 0);
        commandData = json();
    }
}

// Função para receber dados via UDP
void* receiveUDP(void* arg) {
    struct sockaddr_in serverAddr, clientAddr;
    socklen_t clientLen;
    char buffer[MAX_BUFFER_SIZE];

    // Criação do socket
    if (sockUDP < 0) {
        perror("Erro ao abrir socket UDP para recebimento");
        pthread_exit(NULL);
    }

    // Configuração do endereço do servidor UDP
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(UDP_PORT);
    serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);

    // Associação do socket ao endereço do servidor UDP
    if (bind(sockUDP, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {
        perror("Erro ao associar socket ao endereço UDP");
        close(sockUDP);
        pthread_exit(NULL);
    }

    // Recebimento de dados
    while (true) {
        clientLen = sizeof(clientAddr);
        int recvlen = recvfrom(sockUDP, buffer, MAX_BUFFER_SIZE, 0, (struct sockaddr*)&clientAddr, &clientLen);
        if (recvlen > 0) {
            buffer[recvlen] = '\0';

            // Convertendo a string recebida para um objeto JSON
            json receivedData = json::parse(buffer);

            if(receivedData["id"] > (- 1)){
                bool idFind = false;
                if(devices.empty()){
                    devices.push_back(receivedData);
                }
                for (auto& device : devices) {
                    if (device["id"] == receivedData["id"]) {
                        device = receivedData; // Substituir o JSON existente pelo novo JSON
                        idFind = true;
                        break;
                    }
                }

                // Se o ID não foi encontrado, adicionar o novo JSON
                if (!idFind) {
                    devices.push_back(receivedData);
                }
            }

            // Imprimindo o JSON recebido
            std::cout << "Device: " << receivedData << std::endl;
        } else if (recvlen == 0) {
            std::cerr << "Conexão fechada pelo cliente" << std::endl;
            break;
        } else {
            perror("Erro ao receber dados via UDP");
        }
    }

    // Fechar socket e finalizar thread
    close(sockUDP);
    pthread_exit(NULL);
}

int main() {
    pthread_t receiveThread, httpThread, connectionsThread;

    // Criar thread para estabelecer conexões
    pthread_create(&connectionsThread, NULL, establishConnections, NULL);

    // Criando thread para o servidor HTTP
    pthread_create(&httpThread, NULL, runServer, NULL);
    
    // Criando threads para envio e recebimento
    pthread_create(&receiveThread, NULL, receiveUDP, NULL);

    // Esperar pelo término das threads
    pthread_join(receiveThread, NULL);
    pthread_join(httpThread, NULL);
    pthread_join(connectionsThread, NULL);
    return 0;
}
