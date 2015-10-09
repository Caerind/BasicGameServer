#ifndef CONNECTION_HPP
#define CONNECTION_HPP

#include <queue>
#include <iostream>

#include <SFML/System.hpp>
#include <SFML/Network.hpp>


class Connection
{
    public:
        Connection();
        virtual ~Connection();

        bool poll(sf::Packet& packet);
        void send(sf::Packet& packet);

        bool isConnected() const;
        bool connect();
        void disconnect();

        sf::Uint32 getId() const;

        virtual sf::IpAddress getRemoteAddress() const = 0;

        sf::TcpSocket& getSocketIn();
        sf::TcpSocket& getSocketOut();

    protected:
        bool mConnected;

        sf::TcpSocket mSocketIn;
        void receive();
        sf::Thread mReceiveThread;
        sf::Mutex mReceiveMutex;
        std::queue<sf::Packet> mIncoming;

        sf::TcpSocket mSocketOut;
        void send();
        sf::Thread mSendThread;
        sf::Mutex mSendMutex;
        std::queue<sf::Packet> mOutgoing;

        static sf::Uint32 mNumberOfCreations;
        const sf::Uint32 mId;
};

#endif // CONNECTION_HPP
