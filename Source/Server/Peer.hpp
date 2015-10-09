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
        virtual sf::IpAddress getRemoteAddress() const;

        bool isConnected() const;
        bool hasTimedOut() const;
        std::string getUsername() const;

        void timedOut();

    protected:
        bool mConnected;
        bool mTimedOut;
        std::string mUsername;
};

#endif // PEER_HPP
