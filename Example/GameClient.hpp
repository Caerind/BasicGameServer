#ifndef GAMECLIENT_HPP
#define GAMECLIENT_HPP

// Standards Libs
#include <iostream>
#include <string>

// SFML Network
#include <SFML/Network/TcpListener.hpp>

// Own files
#include "../Source/Client.hpp"
#include "PacketType.hpp"

class GameClient : public Client
{
    public:
        GameClient();
        virtual ~GameClient();

        bool connect(sf::IpAddress const& ip, sf::Uint32 port, std::string const& username, std::string const& password);
        void disconnect(bool sendPacket);

        void handleChat();

        std::string getUsername() const;

    protected:
        std::string mUsername;
};

#endif // GAMECLIENT_HPP
