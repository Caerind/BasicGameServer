#ifndef PEER_HPP
#define PEER_HPP

#include <memory>

#include <SFML/Network/TcpSocket.hpp>
#include <SFML/System/Clock.hpp>
#include <SFML/System/Time.hpp>

class Peer
{
    public:
        Peer();

        typedef std::unique_ptr<Peer> Ptr;

        sf::TcpSocket& getSocket();
        sf::Socket::Status receive(sf::Packet& packet);
        sf::Socket::Status send(sf::Packet& packet);

        unsigned int getId() const;
        sf::Time getLastPacketTime() const;
        bool isReady() const;
        bool isConnected() const;
        bool hasTimedOut() const;
        std::string getName() const;

        void setReady(bool ready);
        void setTimedOut(bool timedOut);
        void setName(std::string const& name);

    protected:
        static unsigned int mIdCounter;

        sf::TcpSocket mSocket;

        unsigned int mId;
        sf::Clock mLastPacketTime;
        bool mReady;
        bool mTimedOut;
        std::string mName;
};

#endif // PEER_HPP
