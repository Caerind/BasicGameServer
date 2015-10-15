#ifndef GAMECLIENT_HPP
#define GAMECLIENT_HPP

#include "../Source/Client.hpp"

class GameClient : public Client
{
    public:
        GameClient();

        void handleChat();
};

#endif // GAMECLIENT_HPP
