#include "NetworkClient.h"
#include <boost/archive/text_oarchive.hpp>
#include <iostream>

#include <boost/exception/diagnostic_information.hpp> 


NetworkClient::NetworkClient(const char* host, const char* port) : socket(ioContext) {

    // Init queue with game state
    //gameStates.push(new GameState());
    //gameStates.front()->init();

    boost::asio::ip::tcp::resolver resolver(ioContext);
    boost::asio::ip::tcp::resolver::results_type endpoints = resolver.resolve(host, port);

    try {
        std::cout << "Connecting to server at " << std::string(host) << ":" << port << std::endl;
        boost::asio::connect(socket, endpoints);        // Blocking call, until connect successful/fail
        std::cout << "Server connected" << std::endl;

        /*
        boost::asio::async_read(socket,
            boost::asio::buffer(recvBuf, 4096),
            boost::bind(&NetworkClient::handleReceive, this,
                boost::asio::placeholders::error,
                boost::asio::placeholders::bytes_transferred));
                */
        /*socket.async_read_some(
            boost::asio::buffer(recvBuf),
            boost::bind(&NetworkClient::handleReceive, this,
                boost::asio::placeholders::error,
                boost::asio::placeholders::bytes_transferred));*/
        boost::asio::async_read_until(socket, readStreamBuf, CRLF,
            boost::bind(&NetworkClient::handleReceive, this,
                boost::asio::placeholders::error,
                boost::asio::placeholders::bytes_transferred));
    }
    catch (std::exception& e) {
        std::cerr << "Fail to connect server" << std::endl;
    }
}

void NetworkClient::sendMessage(int opCode) {
    //std::cout << "sendMessage() :"<< opCode << std::endl;
    Message msg(opCode);
    boost::asio::post(ioContext,
        boost::bind(&NetworkClient::doWrite, this, msg));
}

void NetworkClient::doWrite(Message& msg) {
    char buffer[max_length];

    boost::iostreams::basic_array_sink<char> sr(buffer, max_length);
    boost::iostreams::stream< boost::iostreams::basic_array_sink<char> > source(sr);

    boost::archive::text_oarchive oa(source);

    oa << msg;
    source << CRLF;
    source << '\0';

    size_t send_size = std::strlen(buffer);
    socket.async_write_some(
        boost::asio::buffer(buffer, send_size),
        boost::bind(&NetworkClient::handleSend, this,
            boost::asio::placeholders::error,
            boost::asio::placeholders::bytes_transferred));
   /* boost::asio::async_write(socket,
        boost::asio::buffer(msg.data(), msg.length()),
        boost::bind(&NetworkClient::handleSend, this,
            boost::asio::placeholders::error,
            boost::asio::placeholders::bytes_transferred));
            */
}

void NetworkClient::handleSend(const boost::system::error_code& error, size_t bytes_transferred) {
    // TODO:
    if (error) {
        std::cerr << "Message Sent Error" << std::endl;
    }
    //std::cout << "Message Sent" << std::endl;
}

void NetworkClient::handleReceive(const boost::system::error_code& error, size_t bytes_transferred) {
    if (!error) {
        std::string s{
            boost::asio::buffers_begin(readStreamBuf.data()),
            boost::asio::buffers_begin(readStreamBuf.data()) + bytes_transferred - 4 };
        readStreamBuf.consume(bytes_transferred);

        //std::cout << s << std::endl;

        boost::iostreams::stream<boost::iostreams::array_source> is(s.data(), s.length());
        
        boost::archive::text_iarchive ia(is);
        GameState* gameState = new GameState();

        ia >> (*gameState);
        

        {
           boost::lock_guard<boost::recursive_mutex> lock(m_guard);
           gameStates.push(gameState);
        }

        boost::asio::async_read_until(socket, readStreamBuf, CRLF,
            boost::bind(&NetworkClient::handleReceive, this,
                boost::asio::placeholders::error,
                boost::asio::placeholders::bytes_transferred));

       /*socket.async_read_some(
           boost::asio::buffer(recvBuf),
           boost::bind(&NetworkClient::handleReceive, this,
               boost::asio::placeholders::error,
               boost::asio::placeholders::bytes_transferred)); */
    }
    else {
        try {
            socket.close();
        }
        catch (boost::exception const& e) {
            // If exception occurs, this might provide more information
            std::cout << boost::diagnostic_information(e) << std::endl;
        }
    }
}

NetworkClient::~NetworkClient() {
    socket.close();
    t.join();
}

GameState* NetworkClient::getCurrentState() {
    GameState* retVal;
    {
        boost::lock_guard<boost::recursive_mutex> lock(m_guard);

        if (gameStates.size() < 1) {
            return nullptr;
        }
           
        while (gameStates.size() > 1) {
            //std::cout << "gameStates.length() > 1" << std::endl;
            retVal = gameStates.front();
            gameStates.pop();
            delete retVal;
        }
        retVal = gameStates.front();
    }
    return retVal;
}

void NetworkClient::start() {
    t = boost::thread(boost::bind(&boost::asio::io_context::run, &ioContext));
}

void NetworkClient::close() {
    boost::asio::post(ioContext,
        boost::bind(&NetworkClient::doClose, this));
}

void NetworkClient::doClose() {
    socket.close();
    t.join();
}