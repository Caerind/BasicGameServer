#ifndef GAMEPEER_HPP
#define GAMEPEER_HPP

// Standards Libs
#include <string>

// Own files
#include "../Source/Peer.hpp"
#include "../Source/Server.hpp"

class GamePeer : public Peer
{
    public:
        typedef std::shared_ptr<GamePeer> Ptr;

        GamePeer();
        virtual ~GamePeer();

        virtual bool connect(Server<GamePeer>& server);

        std::string getUsername() const;

    protected:
        std::string mUsername;
};

#endif // GAMEPEER_HPP
