#include <string>
#include <iostream>
#include <SFML/Network/IpAddress.hpp>
#include "Client/Client.hpp"
#include "Server/Server.hpp"

int main(int argc, char *argv[])
{
    if (argc == 1)
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
        Server server("server.properties","server.log");
        server.handleAdminInput();
    }
    return EXIT_SUCCESS;
}




