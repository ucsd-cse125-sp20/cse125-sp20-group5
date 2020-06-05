#include "GameServer.h"
#include <iostream>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/iostreams/stream.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include "GameState.hpp"
#include "Message.hpp"
#include "ServerParams.h"

PtrClientConnection ClientConnection::create(boost::asio::io_context& io_context, IGameServer* ptrServer) {
    return PtrClientConnection(new ClientConnection(io_context, ptrServer));
}

ClientConnection::ClientConnection(boost::asio::io_context& io_context, IGameServer* ptrServer)
  : socket_(io_context),
    server(ptrServer), startMessageSent(false) {

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
    size_t send_size = std::strlen(buf);
    //std::cout << buf << std::endl;
    socket_.async_write_some(
        boost::asio::buffer(buf, send_size),
        boost::bind(&ClientConnection::handleWrite, shared_from_this(),
            boost::asio::placeholders::error,
            boost::asio::placeholders::bytes_transferred));
    
/*boost::asio::async_write_some(socket_, boost::asio::buffer(buf, send_size),
        boost::bind(&ClientConnection::handleWrite, shared_from_this(),
            boost::asio::placeholders::error,
            boost::asio::placeholders::bytes_transferred));
*/
}

void ClientConnection::stop(const boost::system::error_code& error) {
    PtrClientConnection pConn = shared_from_this();
    socket_.close();
    server->onClientDisconnected(pConn, error);
}

void ClientConnection::doRead() {
    boost::asio::async_read_until(socket_, readStreamBuf, CRLF,
        boost::bind(&ClientConnection::handleRead, shared_from_this(),
            boost::asio::placeholders::error,
            boost::asio::placeholders::bytes_transferred));
    /*socket_.async_read_some(boost::asio::buffer(readBuf),
        boost::bind(&ClientConnection::handleRead, shared_from_this(),
            boost::asio::placeholders::error,
            boost::asio::placeholders::bytes_transferred));*/
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

    std::string s{
        boost::asio::buffers_begin(readStreamBuf.data()),
        boost::asio::buffers_begin(readStreamBuf.data()) + bytes_transferred - 4 };
    readStreamBuf.consume(bytes_transferred);

    //std::cout << s << std::endl;
    //std::cout << bytes_transferred << std::endl;

    server->onDataRead(shared_from_this(), s.data(), bytes_transferred);
    doRead();
}

void ClientConnection::handleWrite(const boost::system::error_code& error, size_t bytes_transferred) {
    if (error) {
        std::cout << "Serialization Sent Error" << std::endl;
        return;
    }
    //std::cout << "Serialization Sent bytes_transferred = " << bytes_transferred << std::endl;
}

bool ClientConnection::getStartMessageSent() { return startMessageSent; }
void ClientConnection::setStartMessageSent(bool sent) { startMessageSent = sent; }

GameServer::GameServer(
    ServerParams& config,
    boost::asio::io_context& io_context)
  : config(config),
    ioContext(io_context),
    tcpAcceptor(io_context, tcp::endpoint(tcp::v4(), config.port)),
    tickTimer(boost::asio::steady_timer(io_context)),
    deltaTimeMicrosec(1000000 / config.tickrate) {

    gameState.init(config);
    gameStarted = false;
    //gameState.loadFromConfigFile("InitGameState.ini");

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
    if (!clients.empty() && gameStarted) {
        gameState.update();
        char buffer[MAX_BUFFER_SIZE];

        boost::iostreams::basic_array_sink<char> sr(buffer, MAX_BUFFER_SIZE);
        boost::iostreams::stream< boost::iostreams::basic_array_sink<char> > source(sr);

        boost::archive::text_oarchive oa(source);

        oa << gameState;
        source << CRLF;
        source << '\0';


        for (PtrClientConnection conn : clients) {
            conn->deliverSerialization(buffer);
        }
    }

    tickTimer.expires_from_now(boost::asio::chrono::microseconds(deltaTimeMicrosec));
    tickTimer.async_wait(boost::bind(&GameServer::sendToAll, this));
}

void GameServer::onClientConnected(PtrClientConnection pConn) {
    std::cout << "Client connected!" << std::endl;

    // Create new player, don't set values yet
    Player* newPlayer = new Player();
    int opCode;

    // TODO: Update GameState (add player)
    // Need to synchronize?
    {
        boost::lock_guard<boost::recursive_mutex> lock(m_guard);

        // Set new player's values in critical section
        newPlayer->direction = new Direction(Direction::DIRECTION_DOWN);
        newPlayer->animation = new Animation(0, 0);
        newPlayer->objectId = gameState.objectCount++;
        newPlayer->boundingBoxRadius = config.playerCatRadius;
        newPlayer->color = new Color(0, 0, 0);
        newPlayer->playerId = playerIdCounter++;
        newPlayer->maxHealth = config.playerMaxHealth;
        newPlayer->health = newPlayer->maxHealth;

        // Add player to game state
        if (gameStarted) {
            gameState.addPlayer(newPlayer);
        }
        else {
            gameState.addPlayerBeforeStart(newPlayer);
        }

        // Read opcode value here
        opCode  = gameStarted ? OPCODE_GAME_STARTED : OPCODE_GAME_NOT_STARTED;
    }

    // Send initial message to client (has game started or not?)
    char buffer[MAX_BUFFER_SIZE];

    boost::iostreams::basic_array_sink<char> sr(buffer, MAX_BUFFER_SIZE);
    boost::iostreams::stream< boost::iostreams::basic_array_sink<char> > source(sr);

    boost::archive::text_oarchive oa(source);

    Message msg(opCode);
    oa << msg;
    source << CRLF;
    source << '\0';

    pConn->deliverSerialization(buffer);
    pConn->setStartMessageSent(gameStarted);

    // Add player mapping
    pConn2Player.insert(std::make_pair(pConn, newPlayer));
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

    Player* toDel = pConn2Player[pConn];
    pConn2Player.erase(pConn);

    {
        boost::lock_guard<boost::recursive_mutex> lock(m_guard);

        if (gameStarted) {
            gameState.removePlayer(toDel);
        }
        else {
            gameState.removeTempPlayer(toDel);
        }
    }

    // TODO: Update GameState (delete player)
}

void GameServer::onDataRead(PtrClientConnection pConn, const char* pData, size_t bytes_transferred) {
    //pData[bytes_transferred] = '\0';
    //std::cout << "Message received: " << pData << std::endl;
    //std::string msg(pData);

    boost::iostreams::stream<boost::iostreams::array_source> is(pData, MAX_BUFFER_SIZE);
    boost::archive::text_iarchive ia(is);

    Message msg;
    ia >> msg; 

    // if msg is a "level selected" opcode, take string and read the INI file
    if (!gameStarted && msg.getOpCode() == OPCODE_LEVEL_SELECT) { // TODO change this
        int levelId = msg.getLevelId();

        // TODO: use level ID to pick a level on server side
        std::string level;
        if (levelId > config.levels.size()) {
            level = "InitGameState.ini"; // fallback
        }
        else {
            level = config.levels[levelId];
        }

        // Add lock since GameState is modified
        {
            boost::lock_guard<boost::recursive_mutex> lock(m_guard);
            gameState.loadFromConfigFile(level);
            gameStarted = true;
            gameState.setInitPlayerPositions();
        }

        // Send to all waiting clients
        char buffer[MAX_BUFFER_SIZE];

        boost::iostreams::basic_array_sink<char> sr(buffer, MAX_BUFFER_SIZE);
        boost::iostreams::stream< boost::iostreams::basic_array_sink<char> > source(sr);

        boost::archive::text_oarchive oa(source);

        Message msg(OPCODE_GAME_STARTED);
        oa << msg;
        source << CRLF;
        source << '\0';

        for (PtrClientConnection conn : clients) {
            if (!conn->getStartMessageSent()) {
                conn->deliverSerialization(buffer);
                conn->setStartMessageSent(true);
            }
        }
    }
    else if (gameStarted && msg.getOpCode() != OPCODE_LEVEL_SELECT) {

        // Do something with the message (update game state)
        // std::cout << "PtrClientConnection: " << pConn << " Opcode received: " << msg.getOpCode() << std::endl;

        Player* player = pConn2Player[pConn];
        // critical section here
        // TODO: Update gameState
        {
            boost::lock_guard<boost::recursive_mutex> lock(m_guard);
            gameState.updatePlayer(msg.getOpCode(), player);
        }
    }
}

int main(int argc, char* argv[]) {

    try {
        ServerParams serverParams;

        boost::asio::io_context io_context;
        boost::shared_ptr<GameServer> server;
        server.reset(new GameServer(serverParams, io_context));

        io_context.run();
    } catch (std::exception& e) {
        std::cerr << e.what() << std::endl;
    }
}