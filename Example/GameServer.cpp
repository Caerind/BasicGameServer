#include "GameServer.hpp"

GameServer::GameServer()
: Server("server.log")
{
}

void GameServer::load()
{
    Server::load(); // Default values

    // You can load your settings from the opened settings file here
    // Settings

    // Display settings
	write("[Server] Server Version 0.1.1");
    write("[Server]  - Max Players : " + to_string(mMaxPlayers));
	write("[Server]  - Server Ip : " + sf::IpAddress::getPublicAddress().toString());
	write("[Server]  - Server Port : " + to_string(mPort));
}

void GameServer::start()
{
    Server::start(); // Only to show that you can customize it
}

void GameServer::stop()
{
    Server::stop(); // Only to show that you can customize it
}

void GameServer::loadSettings()
{
    // Nothing atm
}

void GameServer::saveSettings()
{
    // Nothing atm
}

void GameServer::createSettings()
{
    // Nothing atm
}

void GameServer::loadFromFile()
{
    std::ifstream file("server.dat");
    if (!file)
    {
        return;
    }
    std::string line;
    while (std::getline(file,line))
    {
        std::string str = line.substr(2);
        switch (line.front())
        {
            case 'a': addAdmin(str); break;
            case 'b': ban(str); break;
            case 'i': banIp(str); break;
            default: break;
        }
    }
    file.close();
}

void GameServer::saveToFile()
{
    std::ofstream file("server.dat");
    if (!file)
    {
        return;
    }
    for (std::size_t i = 0; i < mAdmins.size(); i++)
    {
        file << "a " << mAdmins[i] << std::endl;
    }
    for (std::size_t i = 0; i < mBannedUsers.size(); i++)
    {
        file << "b " << mBannedUsers[i] << std::endl;
    }
    for (std::size_t i = 0; i < mBannedIps.size(); i++)
    {
        file << "i " << mBannedIps[i] << std::endl;
    }
    file.close();
}

bool GameServer::isAdmin(std::string const& username)
{
    for (std::size_t i = 0; i < mAdmins.size(); i++)
    {
        if (mAdmins[i] == username)
        {
            return true;
        }
    }
    return false;
}

void GameServer::addAdmin(std::string const& username)
{
    if (!isAdmin(username))
    {
        mAdmins.push_back(username);
    }
}

void GameServer::removeAdmin(std::string const& username)
{
    for (std::size_t i = 0; i < mAdmins.size(); i++)
    {
        if (mAdmins[i] == username)
        {
            mAdmins.erase(mAdmins.begin() + i);
            return;
        }
    }
}

bool GameServer::isBanned(std::string const& username)
{
    for (std::size_t i = 0; i < mBannedUsers.size(); i++)
    {
        if (mBannedUsers[i] == username)
        {
            return true;
        }
    }
    return false;
}

void GameServer::ban(std::string const& username, std::string const& reason)
{
    if (!isBanned(username))
    {
        mBannedUsers.push_back(username);

        std::string s = "";
        if (reason != "")
        {
            s = " for : " + reason;
        }

        // Tell the user he has been banned
        if (isConnected(username))
        {
            sf::Packet packet;
            Packet::createBannedPacket(packet,Message("", "You have been banned" + s));
            sendToPeer(packet,username);
        }

        // Tell everyone he has been banned
        sf::Packet packet;
        Packet::createServerMessagePacket(packet,Message("", username + " has been banned" + s));
        sendToAll(packet,username);

        write("[Server] " + username + " has been banned" + s);
    }
}

void GameServer::unban(std::string const& username)
{
    for (std::size_t i = 0; i < mBannedUsers.size(); i++)
    {
        if (mBannedUsers[i] == username)
        {
            mBannedUsers.erase(mBannedUsers.begin() + i);
            return;
        }
    }
}

bool GameServer::isBannedIp(sf::IpAddress const& ip)
{
    for (std::size_t i = 0; i < mBannedIps.size(); i++)
    {
        if (mBannedIps[i] == ip)
        {
            return true;
        }
    }
    return false;
}

void GameServer::banIp(sf::IpAddress const& ip, std::string const& reason)
{
    if (!isBannedIp(ip))
    {
        mBannedIps.push_back(ip);

        std::string s = "";
        if (reason != "")
        {
            s = " for : " + reason;
        }

        // Tell the user he has been banned
        sf::Packet packet;
        Packet::createBannedPacket(packet,Message("", "Your IP (" + ip.toString() + ") has been banned" + s));
        sendToIp(packet,ip);

        // Tell everyone ip has been banned
        packet.clear();
        Packet::createServerMessagePacket(packet,Message("", "IP (" + ip.toString() + ") has been banned" + s));
        sendToAll(packet);

        write("[Server] IP (" + ip.toString() + ") has been banned" + s);
    }
}

void GameServer::unbanIp(sf::IpAddress const& ip)
{
    for (std::size_t i = 0; i < mBannedIps.size(); i++)
    {
        if (mBannedIps[i] == ip)
        {
            mBannedIps.erase(mBannedIps.begin() + i);
            return;
        }
    }
}

void GameServer::kick(std::string const& username, std::string const& reason)
{
    if (isConnected(username))
    {
        std::string s = "";
        if (reason != "")
        {
            s = " for : " + reason;
        }

        // Kick him
        sf::Packet packet;
        Packet::createKickedPacket(packet, Message("", "You have been kicked" + s));
        for (std::size_t i = 0; i < mPeers.size(); i++)
        {
            if (mPeers[i]->isConnected() && mPeers[i]->getUsername() == username)
            {
                mPeers[i]->send(packet);
                mPeers[i]->remove();
            }
        }

        // Send to all
        packet.clear();
        Packet::createServerMessagePacket(packet, Message("", username + " has been kicked" + s));
        sendToAll(packet,username);

        write("[Server] " + username + " has been kicked" + s);
    }
}

void GameServer::initCommands()
{
    // Add your commands
}

void GameServer::initPacketResponses()
{
    // Add your packets
}

void GameServer::onConnection(GamePeer& peer)
{
    Server<GamePeer>::onConnection(peer); // Only to show that you can customize it

    std::string username = peer.getUsername();

    sf::Packet packet;
    //Packet::createClientJoinedPacket(packet,username);
    sendToAll(packet);

    write("[Server] " + username + " joined the game");
}

void GameServer::onDisconnection(GamePeer& peer)
{
    Server<GamePeer>::onDisconnection(peer); // Only to show that you can customize it

    std::string username = peer.getUsername();

    sf::Packet packet;
    //Packet::createClientLeftPacket(packet,username);
    sendToAll(packet);

    write("[Server] " + username + " left the game");
}
















/*
void Server::initPacketResponses()
{
    mPacketResponses[Packet::Type::None] = [&](sf::Packet& packet, Peer& peer)
    {
    };

    mPacketResponses[Packet::Type::Login] = [&](sf::Packet& packet, Peer& peer)
    {
    };

    mPacketResponses[Packet::Type::Disconnect] = [&](sf::Packet& packet, Peer& peer)
    {
        peer.remove();
    };

    mPacketResponses[Packet::Type::ClientMessage] = [&](sf::Packet& packet, Peer& peer)
    {
        Message msg;
        Packet::readClientMessagePacket(packet,msg);
        packet.clear();
        if (msg.isCommand())
        {
            std::string res = handleCommand(msg.getContent().substr(1), false, peer.getUsername());
            if (res != "")
            {
                Packet::createServerMessagePacket(packet, Message("",res));
                peer.send(packet);
            }
        }
        else
        {
            Packet::createServerMessagePacket(packet,msg);
            sendToAll(packet);

            write(msg.getEmitter() + " : " + msg.getContent());
        }
    };
}

void Server::initCommands()
{
    mCommands["stop"] = Command("stop",[&](std::string const& args) -> std::string
    {
        stop();
        return "";
    });

    mCommands["help"] = Command("help",[&](std::string const& args) -> std::string
    {
        write("[Server] help : Display the list of commands");
        write("[Server] stop : Stop the server");
        write("[Server] say : Say something");
        write("[Server] ban : Ban an username");
        write("[Server] unban : Unban an username");
        write("[Server] banip : Ban an ip address");
        write("[Server] unbanip : Unban an ip address");
        write("[Server] op : Promote an user to admin rank");
        write("[Server] deop : Demote an user from admin rank");
        return std::string("/help : Display the list of commands\n/say : Say something\n/me : Describe");
    },false);

    mCommands["say"] = Command("say",[&](std::string const& args) -> std::string
    {
        write("[Server] : " + args);

        sf::Packet packet;
        Packet::createServerMessagePacket(packet, Message("[Server]",args));
        sendToAll(packet);

        return "";
    });

    mCommands["ban"] = Command("ban",[&](std::string const& args) -> std::string
    {
        auto a = Command::splitArguments(args);
        if (a.size() >= 1)
        {
            std::string r;
            for (std::size_t i = 1; i < a.size(); i++)
            {
                r += a[i];
            }
            ban(a[0],r);
        }
        return "";
    });

    mCommands["banip"] = Command("banip",[&](std::string const& args) -> std::string
    {
        auto a = Command::splitArguments(args);
        if (a.size() >= 1)
        {
            std::string r;
            for (std::size_t i = 1; i < a.size(); i++)
            {
                r += a[i];
            }
            for (std::size_t i = 0; i < mPeers.size(); i++)
            {
                if (mPeers[i]->isConnected() && mPeers[i]->getUsername() == a[0])
                {
                    banIp(mPeers[i]->getRemoteAddress(),r);
                }
            }
        }
        return "";
    });

    mCommands["unban"] = Command("unban",[&](std::string const& args) -> std::string
    {
        auto a = Command::splitArguments(args);
        if (a.size() >= 1)
        {
            unban(a[0]);
        }
        return "";
    });

    mCommands["unbanip"] = Command("unbanip",[&](std::string const& args) -> std::string
    {
        auto a = Command::splitArguments(args);
        if (a.size() >= 1)
        {
            unbanIp(sf::IpAddress(a[0]));
        }
        return "";
    });

    mCommands["op"] = Command("op",[&](std::string const& args) -> std::string
    {
        auto a = Command::splitArguments(args);
        if (a.size() >= 1)
        {
            addAdmin(a[0]);
        }
        return "";
    });

    mCommands["deop"] = Command("deop",[&](std::string const& args) -> std::string
    {
        auto a = Command::splitArguments(args);
        if (a.size() >= 1)
        {
            removeAdmin(a[0]);
        }
        return "";
    });

    mCommands["kick"] = Command("kick",[&](std::string const& args) -> std::string
    {
        auto a = Command::splitArguments(args);
        if (a.size() >= 1)
        {
            std::string r;
            for (std::size_t i = 1; i < a.size(); i++)
            {
                r += a[i];
            }
            kick(a[0],r);
        }
        return "";
    });

    mCommands["me"] = Command("me",[&](std::string const& args) -> std::string
    {
        write("[Server] : *" + args);

        sf::Packet packet;
        Packet::createServerMessagePacket(packet, Message("", "*" + args));
        sendToAll(packet);

        return "";
    },false);
}
*/
