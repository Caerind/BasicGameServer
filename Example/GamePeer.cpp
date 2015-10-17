#include "GamePeer.hpp"
#include "../Source/Server.hpp"

GamePeer::GamePeer()
: Peer()
{
}

GamePeer::~GamePeer()
{
    disconnect();
}

bool GamePeer::connect(Server& server)
{
    if (!isConnected())
    {
        sf::Packet packet;
        if (getSocketIn().receive(packet) == sf::Socket::Done)
        {
            sf::Int32 packetType;
            packet >> packetType;
            /*if (packetType == Packet::Type::Login)
            {
                sf::IpAddress ip = getSocketIn().getRemoteAddress();
                std::string username, password;
                sf::Uint32 port;
                Packet::readLoginPacket(packet,username,password,port);

                // Test login AND isn't ban AND isn't banip
                // Test login in a db
                if (true && !server.isBanned(username) && !server.isBannedIp(ip) && !server.isConnected(username))
                {
                    if (getSocketOut().connect(ip,port,getTimeout()) == sf::Socket::Status::Done)
                    {
                        mUsername = username;
                        return Connection::connect();
                    }
                }
            }*/
        }
    }
    return isConnected();
}

std::string GamePeer::getUsername() const
{
    return mUsername;
}
