#ifndef CONNECTION_HPP
#define CONNECTION_HPP

#include <queue>

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

        sf::Time getLastReceivePacketTime() const;
        sf::Time getLastSendPacketTime() const;

    protected:
        bool mConnected;

        void receive();
        sf::TcpSocket mSocketIn;
        sf::Thread mReceiveThread;
        sf::Mutex mReceiveMutex;
        sf::Clock mReceiveClock;
        std::queue<sf::Packet> mIncoming;

        void send();
        sf::TcpSocket mSocketOut;
        sf::Thread mSendThread;
        sf::Mutex mSendMutex;
        sf::Clock mSendClock;
        std::queue<sf::Packet> mOutgoing;

        static sf::Uint32 mNumberOfCreations;
        const sf::Uint32 mId;
};

#endif // CONNECTION_HPP
