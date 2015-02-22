#include "../include/Message.hpp"

Message::Message()
{
}

void Message::setTime(std::string const& time)
{
    mTime = time;
}

void Message::setEmitter(std::string const& emitter, sf::Color color)
{
    mEmitter = emitter;
    mEmitterColor = color;
}

void Message::setContent(std::string const& content, sf::Color color)
{
    mContent = content;
    mContentColor = color;
}

std::string Message::getTime() const
{
    return mTime;
}

std::string Message::getEmitter() const
{
    return mEmitter;
}

std::string Message::getContent() const
{
    return mContent;
}

sf::Color Message::getEmitterColor() const
{
    return mEmitterColor;
}

sf::Color Message::getContentColor() const
{
    return mContentColor;
}

void Message::serialize(sf::Packet& packet)
{
    packet << mTime;
    packet << mEmitter;
    packet << mEmitterColor.r << mEmitterColor.g << mEmitterColor.b << mEmitterColor.a;
    packet << mContent;
    packet << mContentColor.r << mContentColor.g << mContentColor.b << mContentColor.a;
}

void Message::unserialize(sf::Packet& packet)
{
    packet >> mTime;
    packet >> mEmitter;
    packet >> mEmitterColor.r >> mEmitterColor.g >> mEmitterColor.b >> mEmitterColor.a;
    packet >> mContent;
    packet >> mContentColor.r >> mContentColor.g >> mContentColor.b >> mContentColor.a;

}
