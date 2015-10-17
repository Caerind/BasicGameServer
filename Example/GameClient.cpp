#include "GameClient.hpp"

GameClient::GameClient()
: Client()
{
    setTimeout(sf::seconds(3.f));
    // initPacketResponses();
}

GameClient::~GameClient()
{
    disconnect(true);
}

bool GameClient::connect(sf::IpAddress const& ip, sf::Uint32 port, std::string const& username, std::string const& password)
{
    if (!isConnected())
    {
        if (getSocketOut().connect(ip,port,getTimeout()) == sf::Socket::Status::Done)
        {
            int retry = 100;
            port += 10;
            sf::TcpListener listener;
            while (listener.listen(port) != sf::Socket::Status::Done && retry > 0)
            {
                ++port;
                --retry;
            }

            if (retry > 0)
            {
                sf::Packet packet;

                // Create Login Packet
                //Packet::createLoginPacket(packet,username,password,port);

                if (getSocketOut().send(packet) == sf::Socket::Done)
                {
                    if (listener.accept(getSocketIn()) == sf::Socket::Done)
                    {
                        mUsername = username;
                        mThread.launch();
                        return Connection::connect();
                    }
                }
            }
        }
    }
    return isConnected();
}

void GameClient::disconnect(bool sendPacket)
{
    if (isConnected())
    {
        if (sendPacket)
        {
            sf::Packet packet;

            // Create Disconnection Packet
            //Packet::createDisconnectPacket(packet);

            send(packet);
        }

        Connection::disconnect();

        mThread.wait();
    }
}

void GameClient::handleChat()
{
    while (isConnected())
    {
        std::string input;
        std::getline(std::cin, input);

        // Handle Chat

        /*

        if (input == "stop" || !isConnected())
        {
            break;
        }

        sf::Packet packet;
        Packet::createClientMessagePacket(packet,Message(getUsername(),input));
        send(packet);

        */
    }
}

std::string GameClient::getUsername() const
{
    return mUsername;
}
