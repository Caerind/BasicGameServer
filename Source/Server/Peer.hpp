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
        typedef std::shared_ptr<Peer> Ptr;

        Peer();
        ~Peer();

        bool connect(Server& server);
        void disconnect();

        void remove();
        bool needRemove() const;

        sf::IpAddress getRemoteAddress() const;

        std::string getUsername() const;

    protected:
        std::string mUsername;
        bool mRemove;
};

#endif // PEER_HPP
