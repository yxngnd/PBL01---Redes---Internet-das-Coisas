#include <boost/asio.hpp>
#include <boost/beast.hpp>
#include <iostream>

namespace asio = boost::asio;              
namespace beast = boost::beast;            
namespace http = beast::http;               

int main() {
    try {
        asio::io_context io_context;

        // Resolver para traduzir o nome do host e a porta em um endpoint TCP
        asio::ip::tcp::resolver resolver(io_context);
        auto const results = resolver.resolve("localhost", "8080");

        // Socket para conectar ao servidor
        asio::ip::tcp::socket socket(io_context);
        asio::connect(socket, results.begin(), results.end());

        // Solicitação HTTP GET
        http::request<http::string_body> req{http::verb::get, "/", 11};
        req.set(http::field::host, "localhost");
        req.set(http::field::user_agent, "Boost Beast Client");

        // Enviar a solicitação
        http::write(socket, req);

        // Receber e imprimir a resposta
        beast::flat_buffer buffer;
        http::response<http::string_body> res;
        http::read(socket, buffer, res);
        std::cout << "Response body: " << res.body() << std::endl;

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
}