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

        void run();
        void stop();
        void wait();

        bool poll(sf::Packet& packet);
        void send(sf::Packet& packet);

        void disconnect();
        int getId() const;

        virtual sf::IpAddress getRemoteAddress() const = 0;

        sf::TcpSocket& getSocketIn();
        sf::TcpSocket& getSocketOut();

    protected:
        bool mRunning;

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

        static int mNumberOfCreations;
        const int mId;
};

#endif // CONNECTION_HPP
