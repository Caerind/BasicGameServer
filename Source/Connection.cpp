#include "Connection.hpp"

int Connection::mNumberOfCreations = 0;

Connection::Connection()
: mRunning(false)
, mReceiveThread(&Connection::receive,this)
, mSendThread(&Connection::send,this)
, mId(++mNumberOfCreations)
{
}

Connection::~Connection()
{
    if (mRunning)
    {
        stop();
        disconnect();
    }
}

void Connection::run()
{
    mRunning = true;
    mReceiveThread.launch();
    mSendThread.launch();
}

void Connection::stop()
{
    mRunning = false;
}

void Connection::wait()
{
    mReceiveThread.wait();
    mSendThread.wait();
}

bool Connection::poll(sf::Packet& packet)
{
    bool res = false;
    sf::Lock guard(mReceiveMutex);
    if (mIncoming.size())
    {
        std::swap(packet,mIncoming.front());
        mIncoming.pop();
        res = true;
    }
    return res;
}

void Connection::send(sf::Packet& packet)
{
    sf::Lock guard(mSendMutex);
    mOutgoing.emplace(packet);
}

void Connection::disconnect()
{
    mSocketIn.disconnect();
    mSocketOut.disconnect();
}

int Connection::getId() const
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

    while(mRunning)
    {
        if(!selector.wait(sf::seconds(1.f)))
            continue;

        if(!selector.isReady(mSocketIn))
            continue;

        sf::Packet packet;
        if(mSocketIn.receive(packet) == sf::Socket::Done)
        {
            sf::Lock guard(mReceiveMutex);
            mIncoming.emplace(std::move(packet));
        }
    }
}

void Connection::send()
{
    while(mRunning)
    {
        mSendMutex.lock();
        if(mOutgoing.size() > 0)
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
