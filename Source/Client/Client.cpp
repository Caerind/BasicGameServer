#include "Client.hpp"

Client::Client()
: mThread(&Client::handlePackets,this)
, mConnected(false)
{
}

Client::~Client()
{
    if (mRunning)
    {
        disconnect();
    }
}

bool Client::connect(sf::IpAddress ip, sf::Uint32 port, std::string const& username, std::string const& password)
{
    std::cout << "Connecting to server : " << ip << std::endl;
    if (mSocketOut.connect(ip,port,sf::seconds(5.f)) == sf::Socket::Status::Done)
    {
        int retry = 100;
        port += 10;
        sf::TcpListener socketListener;
        while (socketListener.listen(port) != sf::Socket::Status::Done && retry > 0)
        {
            ++port;
            --retry;
        }

        if (retry > 0)
        {
            sf::Packet packet;
            Packet::createLoginPacket(packet,username,password,port);
            if (mSocketOut.send(packet) == sf::Socket::Done)
            {
                if (socketListener.accept(mSocketIn) == sf::Socket::Done)
                {
                    std::cout << "Successfully connected !" << std::endl;
                    mConnected = true;
                    mUsername = username;
                    run();
                    mThread.launch();
                    return true;
                }
            }
        }
    }
    std::cout << "Can't connect..." << std::endl;
    return false;
}

void Client::disconnect()
{
    if (isConnected())
    {
        sf::Packet packet;
        Packet::createDisconnectPacket(packet);
        send(packet);

        mConnected = false;
        mUsername = "";

        Connection::disconnect();

        stop();

        std::cout << "Disconnected" << std::endl;
    }
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
            switch (packetType)
            {
                case Packet::ClientJoined:
                {
                    std::string username;
                    Packet::readClientJoinedPacket(packet,username);
                    std::cout << username + " joined the game" << std::endl;
                } break;

                case Packet::ClientLeft:
                {
                    std::string username;
                    Packet::readClientLeftPacket(packet,username);
                    std::cout << username + " left the game" << std::endl;
                } break;

                case Packet::ServerStopped:
                {
                    std::cout << "Server stopped..." << std::endl;
                    disconnect();
                } break;

                case Packet::ServerMessage:
                {
                    Message msg;
                    Packet::readServerMessagePacket(packet,msg);
                    if (msg.getEmitter() != "")
                    {
                        std::cout << msg.getEmitter() << " : " << msg.getContent() << std::endl;
                    }
                    else
                    {
                        std::cout << msg.getContent() << std::endl;
                    }
                } break;

                case Packet::Banned:
                {
                    Message msg;
                    Packet::readBannedPacket(packet,msg);
                    std::cout << msg.getEmitter() << " : " << msg.getContent() << std::endl;
                    disconnect();
                } break;

                case Packet::Kicked:
                {
                    Message msg;
                    Packet::readKickedPacket(packet,msg);
                    std::cout << msg.getEmitter() << " : " << msg.getContent() << std::endl;
                    disconnect();
                } break;

                default: break;
            }
        }
    }
}

void Client::handleChat()
{
    while (isConnected())
    {
        std::string command;
        std::getline(std::cin, command);

        Message msg;
        msg.setEmitter(getUsername());
        msg.setContent(command);

        sf::Packet packet;
        Packet::createClientMessagePacket(packet,msg);
        send(packet);
    }
}

sf::IpAddress Client::getRemoteAddress() const
{
    return mSocketOut.getRemoteAddress();
}

bool Client::isConnected() const
{
    return mConnected;
}

std::string Client::getUsername() const
{
    return mUsername;
}



