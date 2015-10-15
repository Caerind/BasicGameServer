#ifndef GAMESERVER_HPP
#define GAMESERVER_HPP

#include "../Source/Server.hpp"

class GameServer : public Server
{
    public:
        GameServer();

        void load();
        void start();
        void stop();

        void createSettings();

    protected:
        void initCommands();
        void initPacketResponses();

        void onConnection(Peer& peer);
        void onDisconnection(Peer& peer);
};

#endif // GAMESERVER_HPP
