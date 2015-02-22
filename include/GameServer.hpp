#ifndef GAMESERVER_HPP
#define GAMESERVER_HPP

#include "Server.hpp"

class GameServer : public Server
{
    public:
        GameServer(Output* output);

    protected:
        void handlePacket(sf::Packet& packet, Peer& peer, bool& detectedTimeout);
};

#endif // GAMESERVER_HPP
