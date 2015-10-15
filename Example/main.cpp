#include "GameClient.hpp"
#include "GameServer.hpp"

int main(int argc, char *argv[])
{
    if (argc != 1)
    {
        GameClient client;
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




