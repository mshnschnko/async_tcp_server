#include <cstdlib>
#include <cstring>
#include <string>
#include <iostream>
#include <thread>
#include <boost/asio.hpp>
#include <boost/program_options.hpp>

using boost::asio::ip::tcp;

enum { max_length = 1024 };

void run(std::string host, std::string port, int thread_number, int loops) {
    try {
        boost::asio::io_context io_context;
        tcp::socket socket(io_context);
        tcp::resolver resolver(io_context);
        boost::asio::connect(socket, resolver.resolve(host, port));
        std::string reply_prefix = "Reply is: ";
        for (auto i = 0; i < loops; ++i) {
            std::string message = "hello from " + std::to_string(thread_number) + ", time " + std::to_string(i+1);
            std::cout << message << std::endl;
            boost::asio::write(socket, boost::asio::buffer(message.c_str(), message.size() + reply_prefix.size()));
            char reply[max_length];
            boost::asio::read(socket, boost::asio::buffer(reply, message.size() + reply_prefix.size()));
            std::cout << reply << std::endl;
        }
    }
    catch (std::exception &e) {
        std::cerr << "Exception: " << e.what() << std::endl;
    }
}

int main(int argc, const char* argv[]) {
    try {
        std::string host, port;
        int threads_count, loops;
        namespace po = boost::program_options;
        po::options_description desc("Allowed options");
        desc.add_options()
            ("help,h", "Show help")
            ("host,H", po::value<std::string>(&host)->required(), "Host address")
            ("port,P", po::value<std::string>(&port)->required(),"Connect to the port")
            ("threads,T", po::value<int>(&threads_count)->required(),"Number of threads")
            ("loops,L", po::value<int>(&loops)->required(),"Loops in one thread");
    
        po::variables_map vm;
        po::store(po::command_line_parser(argc, argv).options(desc).run(), vm);
        if (vm.count("help")) {
            std::cout << "Usage: client [options]\n";
            std::cout << desc;
            return 0;
        }
        po::notify(vm);

        std::vector<std::thread> threads;
        threads.reserve(threads_count);
        for (auto i = 0; i < threads_count; ++i)
            threads.push_back(std::thread(run, host, port, i + 1, loops));
        for (auto &t: threads)
            t.join();
    }
    catch (std::exception &e) {
        std::cerr << "Exception: " << e.what() << std::endl;
    }
    return 0;
}