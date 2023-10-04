#include <cstdlib>
#include <cstring>
#include <string>
#include <iostream>
#include <thread>
#include <boost/asio.hpp>

using boost::asio::ip::tcp;

enum { max_length = 1024 };

void run(std::string host, std::string port, int loops) {
    try {
        boost::asio::io_context io_context;
        tcp::socket socket(io_context);
        tcp::resolver resolver(io_context);
        boost::asio::connect(socket, resolver.resolve(host, port));

        for (auto i = 0; i < loops; ++i) {
            std::string message = "hello";
            boost::asio::write(socket, boost::asio::buffer(message.c_str(), message.size()));
            char reply[max_length];
            boost::asio::read(socket, boost::asio::buffer(reply, message.size()));
        }
    }
    catch (std::exception &e) {
        std::cerr << "Exception: " << e.what() << std::endl;
    }
}

int main(int argc, char* argv[]) {
    if (argc != 5) {
        std::cerr << "Usage: tcp_client <host> <port> <threads> <loops>" << std::endl;
        return 1;
    }

    std::vector<std::thread> threads;
    size_t threads_count = std::stoi(argv[3]);
    threads.reserve(threads_count);
    for (auto i = 0; i < threads_count; ++i)
        threads.push_back(std::thread(run, argv[1], argv[2], std::stoi(argv[4])));
    for (auto &t: threads)
        t.join();
    return 0;
}