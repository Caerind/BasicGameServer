#include "../include/GameServer.hpp"

GameServer::GameServer(Output* output)
: Server(output)
{
    output->write("","Game Server Version 0.1");
}

void GameServer::handlePacket(sf::Packet& packet, Peer& peer, bool& detectedTimeout)
{

}
