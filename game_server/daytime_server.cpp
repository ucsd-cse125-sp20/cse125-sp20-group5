#include <ctime>
#include <iostream>
#include <string>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/chrono.hpp>
#include <boost/enable_shared_from_this.hpp>

using boost::asio::ip::tcp;

std::string make_daytime_string() {
    using namespace std;
    time_t now = time(0);
    char buf[256];
    ctime_s(buf, 256, &now);
    return buf;
}

/* Synchronize server
int main() {
    try {
        boost::asio::io_context io_context;
        tcp::acceptor acceptor(io_context, tcp::endpoint(tcp::v4(), 13));

        for (;;) {
            tcp::socket socket(io_context);
            acceptor.accept(socket);

            std::string message = make_daytime_string();
            std::cout << "make_daytime_string() called" << std::endl;

            boost::system::error_code ignored_error;
            boost::asio::write(socket, boost::asio::buffer(message), ignored_error);
        }
    }
    catch (std::exception& e) {
        std::cerr << e.what() << std::endl;
    }

    return 0;
}
*/

/* Asynchronize server */
class tcp_connection : public boost::enable_shared_from_this<tcp_connection> {
public:
    typedef boost::shared_ptr<tcp_connection> pointer;

    static pointer create(boost::asio::io_context& io_context, int tickrate) {
        return pointer(new tcp_connection(io_context, tickrate));
    }

    tcp::socket& socket() {
        return socket_;
    }

    /*
    void start() {
        tick_start_ = boost::chrono::high_resolution_clock::now();
        std::cout << "\nTime tickstart_: " << tick_start_ << std::endl;

        message_ = make_daytime_string();
        std::cout << "make_day_time_string() called" << std::endl;
        boost::asio::async_write(socket_, boost::asio::buffer(message_),
            boost::bind(&tcp_connection::handle_write, shared_from_this(),
            boost::asio::placeholders::error,
            boost::asio::placeholders::bytes_transferred));
    }
    */

    void start() {
        std::cout << "Client connected!" << std::endl;
        do_read();
    }

    void deliver(const std::string& msg) {

        boost::asio::async_write(socket_, boost::asio::buffer(msg.data(), msg.length()),
            boost::bind(&tcp_connection::handle_write, shared_from_this(),
                boost::asio::placeholders::error,
                boost::asio::placeholders::bytes_transferred));
    }

private:
    tcp_connection(boost::asio::io_context& io_context, int tickrate)
      : socket_(io_context),
        tickrate_(tickrate),
        timer_(io_context),
        delta_time_(boost::chrono::duration<double>(1.0 / tickrate)){}

    void stop() {
        std::cout << "Client disconnect." << std::endl;
        socket_.close();
    }

    void do_read() {
        socket_.async_read_some(boost::asio::buffer(read_buf_),
            boost::bind(&tcp_connection::handle_read, shared_from_this(),
                boost::asio::placeholders::error,
                boost::asio::placeholders::bytes_transferred));
        /*
        boost::asio::async_read(socket_, boost::asio::buffer(read_buf_, 1),
            boost::bind(&tcp_connection::handle_read, shared_from_this(),
                boost::asio::placeholders::error,
                boost::asio::placeholders::bytes_transferred));
                */
    }

    void handle_write(const boost::system::error_code& error, size_t bytes_transferred) {
        if (error) {
            return;
        }
    }

    void handle_read(const boost::system::error_code& error, size_t bytes_transferred) {
        if (error) {
            stop();
            return;
        }
        std::cout << "Message received: " << read_buf_ << std::endl;
        // put lock here
        do_read();
    }

    boost::asio::io_context io_context_;
    tcp::socket socket_;
    std::string message_;
    enum { max_msg = 1024 };
    char read_buf_[max_msg];
    char write_buf_[max_msg];
    int tickrate_;
    boost::chrono::duration<double> delta_time_;
    boost::chrono::time_point<boost::chrono::high_resolution_clock> tick_start_;
    boost::asio::high_resolution_timer timer_;
};

typedef boost::shared_ptr<tcp_connection> tcp_connection_ptr;

class tcp_server {
public:
    tcp_server(boost::asio::io_context& io_context, int tickrate)
        : io_context_(io_context),
        acceptor_(io_context, tcp::endpoint(tcp::v4(), 13)),
        timer_(boost::asio::steady_timer(io_context)),
        tickrate_(tickrate),
        delta_time_microsec_(1000000 / tickrate) {

        tcp_connection::pointer new_connection = tcp_connection::create(io_context_, tickrate_);
        clients_.push_back(new_connection);
        host = new_connection;
        acceptor_.async_accept(
            new_connection->socket(),
            boost::bind(&tcp_server::handle_accept,
                this,
                boost::asio::placeholders::error));
        send_to_all();
    }

private:

    void handle_accept(const boost::system::error_code& error) {
        if (!error) {
            clients_.back()->start();
        }
        tcp_connection::pointer new_connection = tcp_connection::create(io_context_, tickrate_);
        clients_.push_back(new_connection);
        acceptor_.async_accept(
            new_connection->socket(),
            boost::bind(&tcp_server::handle_accept,
                this,
                boost::asio::placeholders::error));
    }

    void send_to_all() {
        std::string message_ = make_daytime_string();
        //std::cout << "make_day_time_string() in send_to_all() called" << std::endl;
        for (tcp_connection_ptr conn : clients_) {
            conn->deliver(message_);
        }

        timer_.expires_from_now(boost::asio::chrono::microseconds(delta_time_microsec_));
        timer_.async_wait(boost::bind(&tcp_server::send_to_all, this));
    }

    boost::asio::io_context& io_context_;
    tcp::acceptor acceptor_;
    std::vector<tcp_connection_ptr> clients_;
    tcp_connection_ptr host;
    boost::asio::steady_timer timer_;
    int tickrate_;
    int delta_time_microsec_;
};

int main() {
    try {
        boost::asio::io_context io_context;
        tcp_server server(io_context, 4);
        io_context.run();
    }
    catch (std::exception& e) {
        std::cerr << e.what() << std::endl;
    }

    return 0;
}