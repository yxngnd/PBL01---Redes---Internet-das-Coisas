#include <iostream>
#include <boost/asio.hpp>
#include <boost/beast.hpp>
#include <nlohmann/json.hpp>
#include <cstdlib>
#include <pthread.h>

// Sequências de escape ANSI para cores de texto
#define RST "\x1b[0m"
#define RED "\e[0;31m"
#define BLU "\e[0;34m"
#define MAG "\e[0;35m"
#define WHT "\e[0;37m"
#define GRN "\e[0;32m"

namespace asio = boost::asio;
namespace ip = boost::asio::ip;
using tcp = boost::asio::ip::tcp;
namespace http = boost::beast::http;

using json = nlohmann::json;

// Pega host da variavel de ambiente
std::string host = getenv("HOST");
//std::string host = "127.0.0.1";
std::string port =  "8080";

// Função de exibição do menu
void showMenu(){
    std::cout << "###Device Management###" << std::endl;
    std::cout << "[1] - Alterar Status (ligado/desligado) " << std::endl;
    std::cout << "[2] - Alterar Intensidade" << std::endl;
    std::cout << "[3] - Alterar Cor" << std::endl;
    std::cout << ">> " << std::endl;
}

// Função responsável por fazer a requisição GET
json doGet() {
    try {
        asio::io_context io_context;
        tcp::resolver resolver(io_context);
        tcp::socket socket(io_context);

        auto endpoints = resolver.resolve(host, port);
        asio::connect(socket, endpoints);

        // Cria o request
        http::request<http::empty_body> request(http::verb::get, "/", 11);
        request.set(http::field::host, host);
        request.set(http::field::user_agent, "HTTP Client");

        http::write(socket, request); // Escreve o request no endpoint

        // Cria e lê a resposta
        boost::beast::flat_buffer buffer;
        http::response<http::string_body> response;
        http::read(socket, buffer, response);

        // Converte o corpo da resposta JSON para um objeto JSON
        json jsonResponse = json::parse(response.body());
        return jsonResponse;
    } catch (std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }
    return json();
    
}

// Função responsável por fazer a requisição POST
void doPost(json requestJson) {
    try {
        asio::io_context io_context;
        tcp::resolver resolver(io_context);
        tcp::socket socket(io_context);

        auto endpoints = resolver.resolve(host, port);
        asio::connect(socket, endpoints);

        json requestData = requestJson;
        std::string requestBody = requestData.dump();

        // Cria o request
        http::request<http::string_body> request(http::verb::post, "/", 11);
        request.set(http::field::host, host);
        request.set(http::field::user_agent, "HTTP Client");
        request.set(http::field::content_type, "application/json");
        request.body() = requestBody;
        request.prepare_payload();

        // Escreve a requisição
        http::write(socket, request);

        // Cria e lê a resposta
        boost::beast::flat_buffer buffer;
        http::response<http::string_body> response;
        http::read(socket, buffer, response);

        json jsonResponse = json::parse(response.body());

    } catch (std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }
}

// Função para mostrar os dispositivos
void *showDevices(void *arg) {
    while (true) {
        std::system("clear");
        // Obter os dados dos dispositivos usando doGet()
        json devices = doGet();

        // Exibir os dados
        std::cout << "ID    LIGADO    INTENSIDADE    COR" << std::endl;
        for (const auto& device : devices["devices"]) {
            std::cout << device["id"] << "       ";
            if(!(device["on"])){
                std::cout << RED << "OFF" << RST << "          ";

            }
            else{
                std::cout << GRN << "ON" << RST << "           ";
            }
            std::cout << device["intensity"] << "%         ";
            if(device["color"] == "White"){
                std::cout << WHT << "White" << RST << std::endl;

            }
            else if(device["color"] == "Blue"){
                std::cout << BLU << "Blue" << RST << std::endl;
            }
            else if(device["color"] == "Red"){
                std::cout << RED << "Red" << RST << std::endl;
            }
            else if(device["color"] == "Purple"){
                std::cout << MAG << "Purple" << RST << std::endl;
            }
        }
        std::cout << std::endl;
        // Esperar por um intervalo antes de obter os dados novamente
        showMenu();
        sleep(2);
        
    }
    return nullptr;
}

// Função que roda o menu, exibe os comando e seleciona o comando, id e valor
void *menu(void *arg) {
    while(true){
        json requestJson;
        int selectId;
        int command = -1;
        while(command != 0){
            std::cin >> command;
            std::cout << "Para qual dispositivo enviar o comando? >> ";
            std::cin >> selectId;
            switch(command) {
                case 1: {
                    bool newState;
                    std::cout << "Digite o novo status (1 para ligado, 0 para desligado)";
                    std::cout << ">> ";
                    std::cin >> newState;
                    requestJson["id"] = selectId;
                    requestJson["command"] = command;
                    requestJson["value"] = newState;
                    break;
                }
                case 2: {
                    int newIntensity;
                    std::cout << "Digite a nova intensidade";
                    std::cout << ">> ";
                    std::cin >> newIntensity;
                    requestJson["id"] = selectId;
                    requestJson["command"] = command;
                    requestJson["value"] = newIntensity;
                    break;
                }
                case 3: {
                    int newColor;
                    std::cout << "Digite a nova cor: [0] - Branco [1] - Vermelho [2] - Roxo [3] - Azul";
                    std::cout << ">> ";
                    std::cin >> newColor;
                    requestJson["id"] = selectId;
                    requestJson["command"] = command;
                    requestJson["value"] = newColor;
                    break;
                }
                default:
                    std::cout << "Opção inválida!" << std::endl;
            }
            doPost(requestJson);
        }
        }
    return nullptr;
}


int main() {

    // Criação de threades de get e menu
    pthread_t getThread, menuThread;
    
    pthread_create(&getThread, NULL, showDevices, NULL);
    pthread_create(&menuThread, NULL, menu, NULL);
    
    pthread_join(getThread, NULL);
    pthread_join(menuThread, NULL);
    return 0;
}