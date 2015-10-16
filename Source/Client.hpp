#ifndef CLIENT_HPP
#define CLIENT_HPP

// Standards Libs
#include <functional>
#include <map>

// Own files
#include "Connection.hpp"

class Client : public Connection
{
    public:
        Client();
        virtual ~Client();

        void handlePackets();

        sf::IpAddress getRemoteAddress();

    protected:
        sf::Thread mThread;

        std::map<sf::Int32,std::function<void(sf::Packet&)>> mPacketResponses;
};

#endif // CLIENT_HPP
