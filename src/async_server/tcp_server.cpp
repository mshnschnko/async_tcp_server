#include <iostream>
#include <string>
#include <memory>
#include <utility>
#include <boost/asio.hpp>
#include <boost/program_options.hpp>

using boost::asio::ip::tcp;

class session : public std::enable_shared_from_this<session> {
public:
    session (tcp::socket socket, int connection_id) : socket_(std::move(socket)), connection_id_(connection_id) {
        std::cout << "connection established:" <<  connection_id_ << std::endl;
    }

    ~session() {
        std::cout << "connection lost:" << connection_id_ << std::endl;
    }

    void start() {
        do_read();
    }

private:
    void do_read() {
        auto self(shared_from_this());
        socket_.async_read_some(boost::asio::buffer(data_, max_length), [this, self](boost::system::error_code ec, std::size_t length) {
            if (!ec)
                do_write(length);
        });
    }

    void do_write(std::size_t length) {
        auto self(shared_from_this());
        boost::asio::async_write(socket_, boost::asio::buffer(data_, length), [this, self](boost::system::error_code ec, std::size_t /*length*/) {
            if (!ec)
                do_read();
        });
    }

    tcp::socket socket_;
    int connection_id_;
    enum { max_length = 1024 };
    char data_[max_length];
};


class server {
public:
    server(boost::asio::io_context& io_context, short port) : acceptor_(io_context, tcp::endpoint(tcp::v4(), port)) {
        connections_ = 0;
        do_accept();
    }

private:
    void do_accept() {
        acceptor_.async_accept([this](boost::system::error_code ec, tcp::socket socket) {
            if (!ec) {
                this->connections_++;
                std::make_shared<session>(std::move(socket), this->connections_)->start();
            }
            do_accept();
        });
    }

    tcp::acceptor acceptor_;
    int connections_;
};


int main(int argc, char* argv[]) {
    try {
        short port;
        namespace po = boost::program_options;
        po::options_description desc("Allowed options");
        desc.add_options()
            ("help,h", "Show help")
            ("port,P", po::value<short>(&port)->required(),"Connect to the port");
    
        po::variables_map vm;
        po::store(po::command_line_parser(argc, argv).options(desc).run(), vm);
        if (vm.count("help")) {
            std::cout << "Usage: client [options]\n";
            std::cout << desc;
            return 0;
        }
        po::notify(vm);
        boost::asio::io_context io_context;
        server s(io_context, port);
        io_context.run();
    }
    catch (std::exception& e) {
        std::cerr << "Exception: " << e.what() << "\n";
    }
    return 0;
}