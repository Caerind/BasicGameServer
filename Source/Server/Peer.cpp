#include "Peer.hpp"
#include "Server.hpp"

Peer::Peer()
: mConnected(false)
, mTimedOut(false)
{
}

bool Peer::connect(Server& server)
{
    sf::Packet packet;
    if (mSocketIn.receive(packet) == sf::Socket::Done)
    {
        sf::Int32 packetType;
        packet >> packetType;
        if (packetType == Packet::Type::Login)
        {
            sf::IpAddress ip = mSocketIn.getRemoteAddress();
            std::string username, password;
            sf::Uint32 port;
            Packet::readLoginPacket(packet,username,password,port);
            if (true && !server.isBanned(username) && !server.isBannedIp(ip)) // test login AND isn't ban AND isn't banip
            {
                if(mSocketOut.connect(ip,port,sf::seconds(5.f)) == sf::Socket::Status::Done)
                {
                    mConnected = true;
                    mTimedOut = false;
                    mUsername = username;
                    run();
                    return true;
                }
            }
        }
    }
    std::cout << "Peer (" << mId << ") can't connect" << std::endl;
    return false;
}

sf::IpAddress Peer::getRemoteAddress() const
{
    return mSocketIn.getRemoteAddress();
}

bool Peer::isConnected() const
{
    return (mConnected && !mTimedOut);
}

bool Peer::hasTimedOut() const
{
    return mTimedOut;
}

std::string Peer::getUsername() const
{
    return mUsername;
}

void Peer::timedOut()
{
    mTimedOut = true;
}

