#include "GameServer.h"
#include <iostream>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/iostreams/stream.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include "GameState.hpp"

PtrClientConnection ClientConnection::create(boost::asio::io_context& io_context, IGameServer* ptrServer) {
    return PtrClientConnection(new ClientConnection(io_context, ptrServer));
}

ClientConnection::ClientConnection(boost::asio::io_context& io_context, IGameServer* ptrServer)
  : socket_(io_context),
    server(ptrServer) {

}

tcp::socket& ClientConnection::socket() {
    return socket_;
}

void ClientConnection::start() {
    server->onClientConnected(shared_from_this());
    doRead();
}

void ClientConnection::deliver(const std::string& msg) {
    boost::asio::async_write(socket_, boost::asio::buffer(msg.data(), msg.length()),
        boost::bind(&ClientConnection::handleWrite, shared_from_this(),
            boost::asio::placeholders::error,
            boost::asio::placeholders::bytes_transferred));
}

void ClientConnection::deliverSerialization(char* buf) {
    boost::asio::async_write(socket_, boost::asio::buffer(buf, 4096),
        boost::bind(&ClientConnection::handleWrite, shared_from_this(),
            boost::asio::placeholders::error,
            boost::asio::placeholders::bytes_transferred));
}

void ClientConnection::stop(const boost::system::error_code& error) {
    PtrClientConnection pConn = shared_from_this();
    socket_.close();
    server->onClientDisconnected(pConn, error);
}

void ClientConnection::doRead() {
    socket_.async_read_some(boost::asio::buffer(readBuf),
        boost::bind(&ClientConnection::handleRead, shared_from_this(),
            boost::asio::placeholders::error,
            boost::asio::placeholders::bytes_transferred));
    /*boost::asio::async_read(socket_, boost::asio::buffer(read_buf_, 1),
        boost::bind(&tcp_connection::handle_read, shared_from_this(),
            boost::asio::placeholders::error,
            boost::asio::placeholders::bytes_transferred));
            */
}

void ClientConnection::handleRead(const boost::system::error_code& error, size_t bytes_transferred) {
    if (error) {
        stop(error);
        return;
    }
    server->onDataRead(shared_from_this(), readBuf, bytes_transferred);
    doRead();
}

void ClientConnection::handleWrite(const boost::system::error_code& error, size_t bytes_transferred) {
    if (error) {
        return;
    }
}

GameServer::GameServer(
    boost::asio::io_context& io_context,
    int port_num,
    int tick_rate)
  : ioContext(io_context),
    tcpAcceptor(io_context, tcp::endpoint(tcp::v4(), port_num)),
    tickTimer(boost::asio::steady_timer(io_context)),
    deltaTimeMicrosec(1000000 / tick_rate){
    startAccept();
    sendToAll();
}

void GameServer::startAccept() {
    PtrClientConnection newConnection = ClientConnection::create(ioContext, this);
    tcpAcceptor.async_accept(
        newConnection->socket(),
        boost::bind(&GameServer::handleAccept,
            this,
            newConnection,
            boost::asio::placeholders::error));
}

void GameServer::handleAccept(PtrClientConnection newConnection, const boost::system::error_code& error) {
    if (!error) {
        newConnection->start();
    }
    startAccept();
}

void GameServer::sendToAll() {
    if (!clients.empty()) {
        char buffer[4096];
        gameState.updatePlayerPosition();

        boost::iostreams::basic_array_sink<char> sr(buffer, 4096);
        boost::iostreams::stream< boost::iostreams::basic_array_sink<char> > source(sr);

        boost::archive::text_oarchive oa(source);

        oa << gameState;

        for (PtrClientConnection conn : clients) {
            conn->deliverSerialization(buffer);
            //conn->deliver(message);
        }
        std::cout << "Serialization Sent: " << make_daytime_string();
    }

    tickTimer.expires_from_now(boost::asio::chrono::microseconds(deltaTimeMicrosec));
    tickTimer.async_wait(boost::bind(&GameServer::sendToAll, this));
}

void GameServer::onClientConnected(PtrClientConnection pConn) {
    std::cout << "Client connected!" << std::endl;
    clients.push_back(pConn);
}

void GameServer::onClientDisconnected(PtrClientConnection pConn, const boost::system::error_code& error) {
    if (error) {
        std::cerr << error.message() << std::endl;
    }
    std::cout << "Client disconnect." << std::endl;
    auto it = std::find(clients.begin(), clients.end(), pConn);
    if (it != clients.end()) {
        clients.erase(it);
    }
}

void GameServer::onDataRead(PtrClientConnection pConn, char* pData, size_t bytes_transferred) {
    pData[bytes_transferred] = '\0';
    std::cout << "Message received: " << pData << std::endl;
    std::string msg(pData);
}

int main(int argc, char* argv[]) {
    if (argc != 3) {
        std::cerr << "Usage: game_server <port> <tickrate>" << std::endl;
        return 1;
    }
    int port = std::atoi(argv[1]);
    int tickrate = std::atoi(argv[2]);

    try {
        boost::asio::io_context io_context;

        boost::shared_ptr<GameServer> server;
        server.reset(new GameServer(io_context, port, tickrate));

        io_context.run();
    } catch (std::exception& e) {
        std::cerr << e.what() << std::endl;
    }
}