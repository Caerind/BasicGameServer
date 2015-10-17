#ifndef PEER_HPP
#define PEER_HPP

// Standards Libs
#include <memory>

// Own files
#include "Connection.hpp"

// Declaration
class Server;

class Peer : public Connection
{
    public:
        typedef std::shared_ptr<Peer> Ptr;

        Peer();
        virtual ~Peer();

        virtual bool connect(Server& server);

        void remove();
        bool needRemove() const;

        sf::IpAddress getRemoteAddress();

    protected:
        bool mRemove;
};

#endif // PEER_HPP