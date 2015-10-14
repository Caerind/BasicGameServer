#include <string>
#include <iostream>
#include <SFML/Network/IpAddress.hpp>
#include "Client/Client.hpp"
#include "Server/GameServer.hpp"

int main(int argc, char *argv[])
{
    if (argc != 1)
    {
        Client client;
        int retry = 3;
        while (retry > 0)
        {
            if (client.connect(sf::IpAddress::LocalHost,4567,"Cmdu76","test"))
            {
                client.handleChat();
                retry = 0;
            }
            else
            {
                retry--;
            }
        }
    }
    else
    {
        GameServer server;
        server.start();
        while (server.isRunning())
        {
            std::string command;
            std::getline(std::cin, command);
            server.handleCommand(command);
        }
    }
    return EXIT_SUCCESS;
}




