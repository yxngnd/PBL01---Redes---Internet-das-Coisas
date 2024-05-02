#include <iostream>
#include <boost/asio.hpp>
#include <boost/beast.hpp>
#include <nlohmann/json.hpp>

namespace asio = boost::asio;
namespace ip = boost::asio::ip;
using tcp = boost::asio::ip::tcp;
namespace http = boost::beast::http;

using json = nlohmann::json;

void doGet(const std::string& host, const std::string& port) {
    try {
        asio::io_context io_context;
        tcp::resolver resolver(io_context);
        tcp::socket socket(io_context);

        auto endpoints = resolver.resolve(host, port);
        asio::connect(socket, endpoints);

        http::request<http::empty_body> request(http::verb::get, "/", 11);
        request.set(http::field::host, host);
        request.set(http::field::user_agent, "HTTP Client");

        http::write(socket, request);

        boost::beast::flat_buffer buffer;
        http::response<http::string_body> response;
        http::read(socket, buffer, response);

        // Converter o corpo da resposta JSON para um objeto JSON
        json jsonResponse = json::parse(response.body());

        std::cout << "Response body:\n" << jsonResponse.dump(4) << std::endl; // Imprime o JSON formatado
    } catch (std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }
}

void doPost(const std::string& host, const std::string& port) {
    try {
        asio::io_context io_context;
        tcp::resolver resolver(io_context);
        tcp::socket socket(io_context);

        auto endpoints = resolver.resolve(host, port);
        asio::connect(socket, endpoints);

        // Criar objeto JSON
        json requestData;
        requestData["command"] = 1;
        requestData["value"] = 0;

        // Serializar o objeto JSON em uma string
        std::string requestBody = requestData.dump();

        // Construir a requisição HTTP
        http::request<http::string_body> request(http::verb::post, "/", 11);
        request.set(http::field::host, host);
        request.set(http::field::user_agent, "HTTP Client");
        request.set(http::field::content_type, "application/json");
        request.body() = requestBody;
        request.prepare_payload();

        // Enviar a requisição
        http::write(socket, request);

        boost::beast::flat_buffer buffer;
        http::response<http::string_body> response;
        http::read(socket, buffer, response);

        // Converter o corpo da resposta JSON para um objeto JSON
        json jsonResponse = json::parse(response.body());

        std::cout << "Response received:\n" << jsonResponse.dump(4) << std::endl; // Imprime o JSON formatado
    } catch (std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }
}

int main() {
    std::string host = "127.0.0.1";
    std::string port = "8080";

    std::cout << "Sending GET request..." << std::endl;
    doGet(host, port);

    std::cout << "\nSending POST request..." << std::endl;
    doPost(host, port);

    return 0;
}
