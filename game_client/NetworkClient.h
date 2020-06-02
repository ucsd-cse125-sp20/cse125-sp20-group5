#pragma once
#include <queue>
#include <boost/asio.hpp>
#include <boost/iostreams/stream.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/thread.hpp>
#include "GameState.hpp"
#include "Message.hpp"

class NetworkClient {
public:
    NetworkClient(const char* host, const char* port);
    ~NetworkClient();

    void sendMessage(int opCode);
    void sendMessage(int opCode, std::string levelName);
    void handleSend(const boost::system::error_code& error, size_t bytes_transferred);
    void handleReceiveBeforeStart(const boost::system::error_code& error, size_t bytes_transferred);
    void handleReceive(const boost::system::error_code& error, size_t bytes_transferred);
    void doClose();

    GameState* getCurrentState();

    void start();
    void close();

    bool getGameStarted();

private:
    void doWrite(Message& msg);

    bool gameStarted;

    boost::asio::io_context ioContext;
    boost::asio::ip::tcp::socket socket;
    boost::recursive_mutex m_guard;
    enum { max_length = 4096 };
    char sendBuf[max_length];
    boost::asio::streambuf readStreamBuf;
    char recvBuf[max_length];

    std::queue<GameState*> gameStates;
    boost::thread t;
};
