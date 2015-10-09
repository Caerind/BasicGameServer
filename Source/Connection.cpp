#include "Connection.hpp"

sf::Uint32 Connection::mNumberOfCreations = 0;

Connection::Connection()
: mConnected(false)
, mReceiveThread(&Connection::receive,this)
, mSendThread(&Connection::send,this)
, mId(++mNumberOfCreations)
{
}

Connection::~Connection()
{
    disconnect();
}

bool Connection::poll(sf::Packet& packet)
{
    sf::Lock lock(mReceiveMutex);
    if (mIncoming.size() > 0)
    {
        std::swap(packet,mIncoming.front());
        mIncoming.pop();
        return true;
    }
    return false;
}

void Connection::send(sf::Packet& packet)
{
    sf::Lock lock(mSendMutex);
    mOutgoing.emplace(packet);
}

bool Connection::isConnected() const
{
    return mConnected;
}

bool Connection::connect()
{
    mConnected = true;
    mReceiveThread.launch();
    mSendThread.launch();
    return mConnected;
}

void Connection::disconnect()
{
    mConnected = false;

    // Stop threads
    mReceiveThread.wait();
    mSendThread.wait();

    // Send last packets
    while (mOutgoing.size() > 0)
    {
        mSocketOut.send(mOutgoing.front());
        mOutgoing.pop();
    }

    // Stop sockets
    mSocketIn.disconnect();
    mSocketOut.disconnect();
}

sf::Uint32 Connection::getId() const
{
    return mId;
}

sf::TcpSocket& Connection::getSocketIn()
{
    return mSocketIn;
}

sf::TcpSocket& Connection::getSocketOut()
{
    return mSocketOut;
}

void Connection::receive()
{
    sf::SocketSelector selector;
    selector.add(mSocketIn);

    while (mConnected)
    {
        if (!selector.wait(sf::seconds(1.f)))
            continue;

        if (!selector.isReady(mSocketIn))
            continue;

        sf::Packet packet;
        if (mSocketIn.receive(packet) == sf::Socket::Done)
        {
            sf::Lock lock(mReceiveMutex);
            mIncoming.emplace(std::move(packet));
        }
    }
}

void Connection::send()
{
    while (mConnected)
    {
        mSendMutex.lock();
        if (mOutgoing.size() > 0)
        {
            sf::Packet packet = mOutgoing.front();
            mOutgoing.pop();
            mSendMutex.unlock();
            mSocketOut.send(packet);
        }
        else
        {
            mSendMutex.unlock();
        }
    }
}
