#ifndef MESSAGE_HPP
#define MESSAGE_HPP

#include <ctime>
#include <string>

#include <SFML/Network/Packet.hpp>

class Message
{
    public:
        Message();

        std::string getEmitter() const;
        void setEmitter(std::string const& emitter);

        std::string getContent() const;
        void setContent(std::string const& content);
        bool isCommand() const;

        sf::Int64 getTime() const;
        void setTime(sf::Int64 time);
        void setActualTime();

    private:
        std::string mEmitter;
        std::string mContent;
        sf::Int64 mTime;
};

sf::Packet& operator <<(sf::Packet& packet, const Message& msg);
sf::Packet& operator >>(sf::Packet& packet, Message& msg);

#endif // MESSAGE_HPP
