#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <iostream>

#include "../Connection.hpp"
#include "../PacketType.hpp"

class Client : public Connection
{
    public:
        Client();
        ~Client();

        bool connect(sf::IpAddress ip, sf::Uint32 port, std::string const& username, std::string const& password);
        void disconnect(bool sendPacket);

        void handlePackets();
        void handleChat();

        sf::IpAddress getRemoteAddress() const;

        std::string getUsername() const;

    protected:
        sf::Thread mThread;

        std::string mUsername;
};

#endif // CLIENT_HPP
