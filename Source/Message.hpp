#ifndef MESSAGE_HPP
#define MESSAGE_HPP

#include <string>

#include <SFML/Network/Packet.hpp>

class Message
{
    public:
        Message();

        std::string getEmitter() const;
        std::string getContent() const;
        void setEmitter(std::string const& emitter);
        void setContent(std::string const& content);

        bool isCommand() const;

    private:
        std::string mEmitter;
        std::string mContent;
};

sf::Packet& operator <<(sf::Packet& packet, const Message& msg);
sf::Packet& operator >>(sf::Packet& packet, Message& msg);

#endif // MESSAGE_HPP
