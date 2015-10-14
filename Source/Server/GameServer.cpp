#include "GameServer.hpp"

GameServer::GameServer()
: Server("server.properties","server.log")
{
}

void GameServer::load()
{
    Server::load(); // Default values

    // You can load your settings from the opened settings file here
    // TODO : Settings

    // Display settings
	*this << "[Server] Server Version 0.1.1";
    *this << "[Server]  - Max Players : " + to_string(mMaxPlayers);
	*this << "[Server]  - Server Ip : " + sf::IpAddress::getPublicAddress().toString();
	*this << "[Server]  - Server Port : " + to_string(mPort);
}

void GameServer::start()
{
    Server::start(); // Only to show that you can customize it
}

void GameServer::stop()
{
    Server::stop(); // Only to show that you can customize it
}

void GameServer::createSettings()
{
    // Nothing atm
}

void GameServer::initCommands()
{
    Server::initCommands();
    // Add your commands
}

void GameServer::initPacketResponses()
{
    Server::initPacketResponses();
    // Add your packets
}

void GameServer::onConnection(Peer& peer)
{
    Server::onConnection(peer); // Only to show that you can customize it
}

void GameServer::onDisconnection(Peer& peer)
{
    Server::onDisconnection(peer); // Only to show that you can customize it
}
