#include "Peer.hpp"
#include "Server.hpp"

Peer::Peer()
{
    mDisconnecting = false;
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
                if (true && !server.isBanned(username) && !server.isBannedIp(ip))
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
        mDisconnecting = true;
        std::cout << " - Peer (" << mId << ") disconnected" << std::endl;
    }
}

bool Peer::disconnecting() const
{
    return mDisconnecting;
}

sf::IpAddress Peer::getRemoteAddress() const
{
    return mSocketIn.getRemoteAddress();
}

std::string Peer::getUsername() const
{
    return mUsername;
}

