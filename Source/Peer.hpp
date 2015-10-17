#ifndef PEER_HPP
#define PEER_HPP

// Own files
#include "Connection.hpp"

class Peer : public Connection
{
    public:
        Peer();
        virtual ~Peer();

        virtual bool connect();

        void remove();
        bool needRemove() const;

        sf::IpAddress getRemoteAddress();

    protected:
        bool mRemove;
};

#endif // PEER_HPP
