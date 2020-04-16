/*
#include <iostream>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/thread/thread.hpp>
*/

/* Tutorial 1
int main(void) {
    boost::asio::io_context io;
    boost::asio::steady_timer t(io, boost::asio::chrono::seconds(5));
    t.wait();

    std::cout << "Hello World!" << std::endl;
    return 0;
}
*/

/* Tutorial 2
void print(const boost::system::error_code& e) {
    std::cout << "Hello World!" << std::endl;
}

int main(void) {
    boost::asio::io_context io;
    boost::asio::steady_timer t(io, boost::asio::chrono::seconds(5));
    t.async_wait(&print);
    io.run();
    return 0;
}
*/

/* Tutorial 3
void print(const boost::system::error_code& e, boost::asio::steady_timer *t, int *count) {
    if (*count < 5) {
        std::cout << *count << std::endl;
        ++(*count);
        t->expires_at(t->expiry() + boost::asio::chrono::seconds(1));
        t->async_wait(boost::bind(print, 
            boost::asio::placeholders::error, t, count));
    }
    std::cout << "Hello World!" << std::endl;
}

int main(void) {
    boost::asio::io_context io;
    int count = 0;
    boost::asio::steady_timer t(io, boost::asio::chrono::seconds(1));
    t.async_wait(boost::bind(print, boost::asio::placeholders::error, &t, &count));
    io.run();

    std::cout << "Final count is " << count << std::endl;
    return 0;
}
*/

/* Tutorial 4
class Printer {
public:
    Printer(boost::asio::io_context& io) : timer_(io, boost::asio::chrono::seconds(1)), count_(0) {
        timer_.async_wait(boost::bind(&Printer::print, this));
    }

    ~Printer() {
        std::cout << "Final count is " << count_ << std::endl;
    }

    void print() {
        if (count_ < 5) {
            std::cout << count_ << std::endl;
            ++count_;

            timer_.expires_at(timer_.expiry() + boost::asio::chrono::seconds(1));
            timer_.async_wait(boost::bind(&Printer::print, this));
        }
    }

private:
    boost::asio::steady_timer timer_;
    int count_;
};

int main(void) {
    boost::asio::io_context io;
    Printer printer(io);
    io.run();
}
*/

/* Tutorial 5
class Printer {
public:
    Printer(boost::asio::io_context& io)
      : strand_(boost::asio::make_strand(io)),
        timer1_(io, boost::asio::chrono::seconds(1)),
        timer2_(io, boost::asio::chrono::seconds(1)),
        count_(0) {
        timer1_.async_wait(boost::asio::bind_executor(strand_, boost::bind(&Printer::print1, this)));
        timer2_.async_wait(boost::asio::bind_executor(strand_, boost::bind(&Printer::print2, this)));
    }

    ~Printer() {
        std::cout << "Final count is " << count_ << std::endl;
    }

    void print1() {
        if (count_ < 10) {
            std::cout << "Timer 1: " << count_ << std::endl;
            ++count_;

            timer1_.expires_at(timer1_.expiry() + boost::asio::chrono::seconds(1));
            timer1_.async_wait(boost::asio::bind_executor(strand_, boost::bind(&Printer::print1, this)));
        }
    }

    void print2() {
        if (count_ < 10) {
            std::cout << "Timer 2: " << count_ << std::endl;
            ++count_;

            timer2_.expires_at(timer2_.expiry() + boost::asio::chrono::seconds(1));
            timer2_.async_wait(boost::asio::bind_executor(strand_, boost::bind(&Printer::print2, this)));
        }
    }

private:
    boost::asio::strand<boost::asio::io_context::executor_type> strand_;
    boost::asio::steady_timer timer1_;
    boost::asio::steady_timer timer2_;
    int count_;
};

int main(void) {
    boost::asio::io_context io;
    Printer printer(io);
    boost::thread t(boost::bind(&boost::asio::io_context::run, &io));
    io.run();
    t.join();
}
*/

/* TCP daytime */
#include <iostream>
#include <boost/array.hpp>
#include <boost/asio.hpp>

using boost::asio::ip::tcp;

int main(int argc, char* argv[]) {
    try {
        if (argc != 2) {
            std::cerr << "Usage: client <host>" << std::endl;
            return 1;
        }

        boost::asio::io_context io_context;
        tcp::resolver resolver(io_context);
        tcp::resolver::results_type endpoints =
            resolver.resolve(argv[1], "daytime");

        tcp::socket socket(io_context);
        boost::asio::connect(socket, endpoints);
        std::string msg("hello");
        for (;;) {
            boost::array<char, 128> buf;
            boost::system::error_code error;

            size_t len = socket.read_some(boost::asio::buffer(buf), error);

            if (error == boost::asio::error::eof) {
                break;
            } else if (error) {
                throw boost::system::system_error(error);
            }
            
            std::cout.write(buf.data(), len);
            len = socket.write_some(boost::asio::buffer(msg.data(), msg.length()), error);
            len = socket.write_some(boost::asio::buffer(msg.data(), msg.length()), error);
        }
        io_context.run();
    }
    catch (std::exception& e) {
        std::cerr << e.what() << std::endl;
    }

    return 0;
}
