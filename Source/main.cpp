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

        if (client.connect(sf::IpAddress::LocalHost,4567,"Cmdu76","test"))
        {

        }
    }
    else
    {
        Server server("server.properties","server.log");
        server.handleAdminInput();
    }
    return EXIT_SUCCESS;
}




