#ifndef GAMEPEER_HPP
#define GAMEPEER_HPP

// Standards Libs
#include <string>

// Own files
#include "../Source/Peer.hpp"

// Declaration
class Server;

class GamePeer : Peer
{
    public:
        typedef std::shared_ptr<GamePeer> Ptr;

        GamePeer();
        virtual ~GamePeer();

        virtual bool connect(Server& server);

        std::string getUsername() const;

    protected:
        std::string mUsername;
};

#endif // GAMEPEER_HPP
