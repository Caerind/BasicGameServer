#ifndef PEER_HPP
#define PEER_HPP

#include <iostream>
#include <memory>

#include "../Connection.hpp"
#include "../PacketType.hpp"

class Server;

class Peer : public Connection
{
    public:
        typedef std::unique_ptr<Peer> Ptr;

        Peer();
        ~Peer();

        bool connect(Server& server);
        void disconnect();

        sf::IpAddress getRemoteAddress() const;

        std::string getUsername() const;

    protected:
        std::string mUsername;
};

#endif // PEER_HPP
