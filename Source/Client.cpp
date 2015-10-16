#include "Client.hpp"

Client::Client()
: mThread(&Client::handlePackets,this)
{
}

Client::~Client()
{
    disconnect();
}

void Client::handlePackets()
{
    while (isConnected())
    {
        sf::Packet packet;
        while (poll(packet))
        {
            sf::Int32 packetType;
            packet >> packetType;

            auto itr = mPacketResponses.find(packetType);
            if (itr != mPacketResponses.end() && itr->second)
            {
                itr->second(packet);
            }
        }
    }
}

sf::IpAddress Client::getRemoteAddress()
{
    return getSocketOut().getRemoteAddress();
}
