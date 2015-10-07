#include "Message.hpp"

Message::Message()
{
}

std::string Message::getEmitter() const
{
    return mEmitter;
}

std::string Message::getContent() const
{
    return mContent;
}

void Message::setEmitter(std::string const& emitter)
{
    mEmitter = emitter;
}

void Message::setContent(std::string const& content)
{
    mContent = content;
}

bool Message::isCommand() const
{
    return mContent.front() == '/';
}

sf::Packet& operator <<(sf::Packet& packet, const Message& msg)
{
    return packet << msg.getEmitter() << msg.getContent();
}

sf::Packet& operator >>(sf::Packet& packet, Message& msg)
{
    std::string e,c;
    packet >> e >> c;
    msg.setEmitter(e);
    msg.setContent(c);
    return packet;
}

