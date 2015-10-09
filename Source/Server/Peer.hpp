#ifndef PEER_HPP
#define PEER_HPP

#include <memory>

#include "../Connection.hpp"
#include "../PacketType.hpp"

class Server;

class Peer : public Connection
{
    public:
        Peer();

        typedef std::unique_ptr<Peer> Ptr;

        bool connect(Server& server);
        sf::IpAddress getRemoteAddress() const;

        std::string getUsername() const;

    protected:
        std::string mUsername;
};

#endif // PEER_HPP
