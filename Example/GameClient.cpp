#include "GameClient.hpp"

GameClient::GameClient()
: Client()
{
}

void GameClient::handleChat()
{
    while (isConnected())
    {
        std::string command;
        std::getline(std::cin, command);

        if (command == "stop" || !isConnected())
        {
            break;
        }

        sf::Packet packet;
        Packet::createClientMessagePacket(packet,Message(getUsername(),command));
        send(packet);
    }
}
