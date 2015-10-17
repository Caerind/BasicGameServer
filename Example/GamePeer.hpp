#ifndef GAMEPEER_HPP
#define GAMEPEER_HPP

// Standards Libs
#include <iostream>
#include <string>

// Own files
#include "../Source/Peer.hpp"
#include "PacketType.hpp"

class GameServer;

class GamePeer : public Peer
{
    public:
        GamePeer();
        virtual ~GamePeer();

        virtual bool connect(GameServer& server);

        std::string getUsername() const;

    protected:
        std::string mUsername;
};

#endif // GAMEPEER_HPP
