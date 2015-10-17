#ifndef PEER_HPP
#define PEER_HPP

// Own files
#include "Connection.hpp"

// Template Declaration
template <typename T>
class Server;

class Peer : public Connection
{
    public:
        Peer();
        virtual ~Peer();

        virtual bool connect(Server<Peer>& server);

        void remove();
        bool needRemove() const;

        sf::IpAddress getRemoteAddress();

    protected:
        bool mRemove;
};

#endif // PEER_HPP
