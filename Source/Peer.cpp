#include "Peer.hpp"
#include "Server.hpp"

Peer::Peer()
: mRemove(false)
{
}

Peer::~Peer()
{
    disconnect();
}

bool Peer::connect(Server<Peer>& server)
{
    return isConnected();
}

void Peer::remove()
{
    mRemove = true;
}

bool Peer::needRemove() const
{
    return mRemove;
}

sf::IpAddress Peer::getRemoteAddress()
{
    return getSocketIn().getRemoteAddress();
}
