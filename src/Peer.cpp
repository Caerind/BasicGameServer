#include "../include/Peer.hpp"

unsigned int Peer::mIdCounter = 0;

Peer::Peer()
{
    mId = mIdCounter++;
    mSocket.setBlocking(false);
}

sf::TcpSocket& Peer::getSocket()
{
    return mSocket;
}

sf::Socket::Status Peer::receive(sf::Packet& packet)
{
    sf::Socket::Status status = mSocket.receive(packet);
    if (status == sf::Socket::Done && !hasTimedOut())
    {
        mLastPacketTime.restart();
    }
    return status;
}

sf::Socket::Status Peer::send(sf::Packet& packet)
{
    return mSocket.send(packet);
}

unsigned int Peer::getId() const
{
    return mId;
}

sf::Time Peer::getLastPacketTime() const
{
    return mLastPacketTime.getElapsedTime();
}

bool Peer::isReady() const
{
    return mReady;
}

bool Peer::isConnected() const
{
    return (mReady && mName != "");
}

bool Peer::hasTimedOut() const
{
    return mTimedOut;
}

std::string Peer::getName() const
{
    return mName;
}

void Peer::setReady(bool ready)
{
    mReady = ready;
    if (mReady)
        mLastPacketTime.restart();
}

void Peer::setTimedOut(bool timedOut)
{
    mTimedOut = timedOut;
}

void Peer::setName(std::string const& name)
{
    mName = name;
}

