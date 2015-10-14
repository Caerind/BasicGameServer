#include "Peer.hpp"
#include "Server.hpp"

Peer::Peer()
{
    mRemove = false;
}

Peer::~Peer()
{
    disconnect();
}

bool Peer::connect(Server& server)
{
    if (!mConnected)
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

                // Test login AND isn't ban AND isn't banip
                // TODO : Test login in a db
                if (true && !server.isBanned(username) && !server.isBannedIp(ip) && !server.isConnected(username))
                {
                    if(mSocketOut.connect(ip,port,sf::seconds(5.f)) == sf::Socket::Status::Done)
                    {
                        std::cout << " - Peer (" << mId << ") connected" << std::endl;
                        mUsername = username;
                        return Connection::connect();
                    }
                }
            }
        }
        std::cout << " - Peer (" << mId << ") can't connect" << std::endl;
    }
    return mConnected;
}

void Peer::disconnect()
{
    if (mConnected)
    {
        Connection::disconnect();
        std::cout << " - Peer (" << mId << ") disconnected" << std::endl;
    }
}

void Peer::remove()
{
    mRemove = true;
}

bool Peer::needRemove() const
{
    return mRemove;
}

sf::IpAddress Peer::getRemoteAddress() const
{
    return mSocketIn.getRemoteAddress();
}

std::string Peer::getUsername() const
{
    return mUsername;
}

