#ifndef MESSAGE_HPP
#define MESSAGE_HPP

#include <string>

#include <SFML/Graphics/Color.hpp>
#include <SFML/Network/Packet.hpp>

class Message
{
    public:
        Message();

        void setTime(std::string const& time);
        void setEmitter(std::string const& emitter, sf::Color color = sf::Color::Black);
        void setContent(std::string const& content, sf::Color color = sf::Color::Black);

        std::string getTime() const;
        std::string getEmitter() const;
        std::string getContent() const;
        sf::Color getEmitterColor() const;
        sf::Color getContentColor() const;

        void serialize(sf::Packet& packet);
        void unserialize(sf::Packet& packet);

    protected:
        std::string mTime;
        std::string mEmitter;
        sf::Color mEmitterColor;
        std::string mContent;
        sf::Color mContentColor;
};

#endif // MESSAGE_HPP
