#ifndef CLIENT_HPP
#define CLIENT_HPP

#include "../Connection.hpp"
#include "../PacketType.hpp"

class Client : public Connection
{
    public:
        Client();
        ~Client();

        bool connect(sf::IpAddress ip, sf::Uint32 port, std::string const& username, std::string const& password);
        void disconnect();

        void handlePackets();
        void handleChat();

        sf::IpAddress getRemoteAddress() const;

        bool isConnected() const;
        std::string getUsername() const;

    protected:
        sf::Thread mThread;


        bool mConnected;
        std::string mUsername;
};

#endif // CLIENT_HPP
