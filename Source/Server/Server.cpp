#include "Server.hpp"

Server::Server(std::string const& propertiesFile, std::string const& logFile)
: mThread(&Server::run,this)
, mRunning(false)
, mListener()
, mListeningState(false)
, mSettingsFile(propertiesFile)
, mConnectedPlayers(0)
, mPeers(1)
{
    sf::Clock clock;

    openLog(logFile);

    *this << "[Server] Loading server";

    mListener.setBlocking(false);
	mPeers[0].reset(new Peer());

    if (!loadSettings())
    {
        createSettings();
        saveSettings();
    }

    load();

    initCommands();
    initPacketResponses();

    loadAdmins();
    loadBans();

    *this << "[Server] Loaded in " + to_string(clock.restart().asSeconds()) + "s !";
}

Server::~Server()
{
    stop();
}

void Server::load()
{
    mMaxPlayers = 10;
    mPort = 4567;
    mClientTimeoutTime = sf::seconds(5.f);
    mUpdateInterval = sf::seconds(1.f/60.f);
}

void Server::start()
{
    mRunning = true;
    mThread.launch();

    *this << "[Server] Server started !";
}

void Server::stop()
{
    if (mRunning)
    {
        *this << "[Server] Stopping server";

        setListening(false);

        sf::Packet packet;
        Packet::createServerStoppedPacket(packet);
        sendToAll(packet);

        mRunning = false;

        mThread.wait();

        *this << "[Server] Server stopped !";

        saveAdmins();
        saveBans();
        saveSettings();
        mLog.close();
    }
}

bool Server::isRunning() const
{
    return mRunning;
}

void Server::sendToAll(sf::Packet& packet, std::string const& excludeUser)
{
    for (std::size_t i = 0; i < mPeers.size(); i++)
    {
        if (mPeers[i]->isConnected() && mPeers[i]->getUsername() != excludeUser)
        {
            mPeers[i]->send(packet);
        }
    }
}

void Server::sendToPeer(sf::Packet& packet, std::string const& username)
{
    auto p = getPeer(username);
    if (p != nullptr)
    {
        if (p->isConnected())
        {
            p->send(packet);
        }
    }
}

bool Server::isConnected(std::string const& username)
{
    auto p = getPeer(username);
    if (p != nullptr)
    {
        if (p->isConnected())
        {
            return true;
        }
    }
    return false;
}

Peer::Ptr Server::getPeer(std::string const& username)
{
    for (std::size_t i = 0; i < mPeers.size(); i++)
    {
        if (mPeers[i] != nullptr)
        {
            if (mPeers[i]->getUsername() == username)
            {
                return mPeers[i];
            }
        }
    }
    return nullptr;
}

std::string Server::handleCommand(std::string const& command, bool server, std::string const& username)
{
    std::vector<std::string> args = Command::getCommandName(command);
    if (args.size() >= 1)
    {
        auto itr = mCommands.find(args[0]);
        if (itr != mCommands.end())
        {
            // TODO : User Permission Level
            // 10 represent user permission level
            if (server || isAdmin(username) || (!itr->second.isAdminOnly() && itr->second.getPermissionLevel() <= 10))
            {
                return itr->second.execute(args[1]);
            }
            else
            {
                return "You don't have the permission to do that";
            }
        }
        else
        {
            if (server)
            {
                *this << "[Server] Unknow command, try \"help\" to see the list of commands";
            }
            return "Unknown command";
        }
    }
    return "";
}

bool Server::isAdmin(std::string const& username)
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

void Server::addAdmin(std::string const& username)
{
    if (!isAdmin(username))
    {
        mAdmins.push_back(username);
    }
}

void Server::removeAdmin(std::string const& username)
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

void Server::loadAdmins(std::string const& adminFile)
{
    std::ifstream file(adminFile);
    if (file)
    {
        std::string line;
        while (std::getline(file,line))
        {
            addAdmin(line);
        }
    }
    file.close();
}

void Server::saveAdmins(std::string const& adminFile)
{
    std::ofstream file(adminFile);
    if (file)
    {
        for (std::size_t i = 0; i < mAdmins.size(); i++)
        {
            file << mAdmins[i] << std::endl;
        }
    }
    file.close();
}

bool Server::isBanned(std::string const& username)
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

bool Server::isBannedIp(sf::IpAddress const& ip)
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

void Server::ban(std::string const& username, std::string const& reason)
{
    if (!isBanned(username))
    {
        mBannedUsers.push_back(username);

        // Tell the user he has been banned
        if (isConnected(username))
        {
            Message msg;
            msg.setEmitter("[Server]");
            if (reason != "")
            {
                msg.setContent("You have been banned for : " + reason);
            }
            else
            {
                msg.setContent("You have been banned");
            }
            sf::Packet packet;
            Packet::createBannedPacket(packet,msg);
            sendToPeer(packet,username);
        }
    }
}

void Server::banIp(sf::IpAddress const& ip, std::string const& reason)
{
    if (!isBannedIp(ip))
    {
        mBannedIps.push_back(ip);

        // Tell the user he has been banned
        Message msg;
        msg.setEmitter("[Server]");
        if (reason != "")
        {
            msg.setContent("You have been banned for : " + reason);
        }
        else
        {
            msg.setContent("You have been banned");
        }
        sf::Packet packet;
        Packet::createBannedPacket(packet,msg);
        for (std::size_t i = 0; i < mPeers.size(); i++)
        {
            if (mPeers[i]->isConnected() && mPeers[i]->getRemoteAddress() == ip)
            {
                mPeers[i]->send(packet);
            }
        }
    }
}

void Server::unban(std::string const& username)
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

void Server::unbanIp(sf::IpAddress const& ip)
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

void Server::loadBans(std::string const& banFile)
{
    std::ifstream file(banFile);
    if (file)
    {
        std::string line;
        while (std::getline(file,line))
        {
            if (line.front() == 'u')
            {
                ban(line.substr(2));
            }
            else if (line.front() == 'i')
            {
                banIp(sf::IpAddress(line.substr(2)));
            }
        }
    }
    file.close();
}

void Server::saveBans(std::string const& banFile)
{
    std::ofstream file(banFile);
    if (file)
    {
        for (std::size_t i = 0; i < mBannedUsers.size(); i++)
        {
            file << "u " << mBannedUsers[i] << std::endl;
        }
        for (std::size_t i = 0; i < mBannedIps.size(); i++)
        {
            file << "i " << mBannedIps[i] << std::endl;
        }
    }
    file.close();
}

void Server::kick(std::string const& username, std::string const& reason)
{
    Message msg;
    msg.setEmitter("[Server]");
    if (reason != "")
    {
        *this << username + " has been kicked for : " + reason;
        msg.setContent("You have been kicked for : " + reason);
    }
    else
    {
        *this << username + " has been kicked";
        msg.setContent("You have been kicked");
    }
    sf::Packet packet;
    Packet::createKickedPacket(packet,msg);
    sendToPeer(packet,username);
    for (std::size_t i = 0; i < mPeers.size(); i++)
    {
        if (mPeers[i]->isConnected() && mPeers[i]->getUsername() == username)
        {
            mPeers[i]->remove();
        }
    }
}

std::string Server::getSettings(std::string const& id) const
{
    if (mSettings.find(id) != mSettings.end())
    {
        return mSettings.at(id);
    }
    else
    {
        return "";
    }
}

bool Server::loadSettings()
{
    std::ifstream file(mSettingsFile);
    if (!file)
    {
        return false;
    }

    std::string line;
    while (std::getline(file,line))
    {
        if(line.size() > 0 && line[0] != '#')
        {
            std::size_t index = 0;
            while(std::isspace(line[index]))
                index++;
            const std::size_t beginKeyString = index;
            while(!std::isspace(line[index]) && line[index] != '=')
                index++;
            const std::string key = line.substr(beginKeyString, index - beginKeyString);
            while(std::isspace(line[index]) || line[index] == '=')
                index++;
            const std::string value = line.substr(index, line.size() - index);

            mSettings[key] = value;
        }
    }

    file.close();
    return true;
}

bool Server::saveSettings()
{
    std::ofstream file(mSettingsFile);
    if (!file)
    {
        return false;
    }

    for (auto itr = mSettings.begin(); itr != mSettings.end(); itr++)
    {
        file << itr->first << "=" << itr->second << std::endl;
    }

    file.close();
    return true;
}

void Server::createSettings()
{
}

std::string Server::getTimeFormat()
{
    time_t rawtime;
    struct tm* timeinfo;
    char buffer[80];
    time (&rawtime);
    timeinfo = localtime(&rawtime);
    std::string timeformat = "[%x][%X] ";
    strftime(buffer,80,timeformat.c_str(),timeinfo);
    return std::string(buffer);
}

bool Server::openLog(std::string const& logFile)
{
    if (logFile != "" && !mLog.is_open())
    {
        mLog.open(logFile);
    }
    return mLog.is_open();
}

bool Server::isLogOpen() const
{
    return mLog.is_open();
}

std::ofstream& Server::getLogStream()
{
    return mLog;
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
        *this << "[Server] help : Display the list of commands";
        *this << "[Server] stop : Stop the server";
        *this << "[Server] say : Say something";
        *this << "[Server] ban : Ban an username";
        *this << "[Server] unban : Unban an username";
        *this << "[Server] banip : Ban an ip address";
        *this << "[Server] unbanip : Unban an ip address";
        *this << "[Server] op : Promote an user to admin rank";
        *this << "[Server] deop : Demote an user from admin rank";
        return std::string("/help : Display the list of commands\n/stop : Stop the server\n/say : Say something");
    },false);

    mCommands["say"] = Command("say",[&](std::string const& args) -> std::string
    {
        *this << "[Server] : " + args;

        Message msg;
        msg.setEmitter("[Server]");
        msg.setContent(args);
        sf::Packet packet;
        Packet::createServerMessagePacket(packet,msg);
        sendToAll(packet);

        return "";
    });

    mCommands["ban"] = Command("ban",[&](std::string const& args) -> std::string
    {
        auto a = Command::splitArguments(args);
        if (a.size() == 1)
        {
            ban(args);
            *this << args + " has been banned";
        }
        else if (a.size() > 1)
        {
            std::string r;
            for (std::size_t i = 1; i < a.size(); i++)
            {
                r += a[i];
            }
            ban(a[0],r);
            *this << a[0] + " has been banned for : " + r;
        }
        return "";
    });

    mCommands["banip"] = Command("banip",[&](std::string const& args) -> std::string
    {
        auto a = Command::splitArguments(args);
        if (a.size() >= 1)
        {
            std::string r = "";
            if (a.size() > 1)
            {
                for (std::size_t i = 1; i < a.size(); i++)
                {
                    r += a[i];
                }
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
        Message msg;
        msg.setEmitter("[Server]");
        if (a.size() >= 1)
        {
            std::string r;
            if (a.size() > 1)
            {
                for (std::size_t i = 1; i < a.size(); i++)
                {
                    r += a[i];
                }
            }
            kick(a[0],r);
        }
        return "";
    });

}

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
                msg.setEmitter("");
                msg.setContent(res);
                Packet::createServerMessagePacket(packet,msg);
                peer.send(packet);
            }
        }
        else
        {
            Packet::createServerMessagePacket(packet,msg);
            sendToAll(packet);

            *this << msg.getEmitter() + " : " + msg.getContent();
        }
    };
}

void Server::onConnection(Peer& peer)
{
    std::string username = peer.getUsername();

    sf::Packet packet;
    Packet::createClientJoinedPacket(packet,username);
    sendToAll(packet);

    *this << "[Server] " + username + " joined the game";
}

void Server::onDisconnection(Peer& peer)
{
    std::string username = peer.getUsername();

    sf::Packet packet;
    Packet::createClientLeftPacket(packet,username);
    sendToAll(packet);

    *this << "[Server] " + username + " left the game";
}

void Server::setListening(bool enable)
{
    if (enable) // Check if it isn't already listening
	{
		if (!mListeningState)
        {
            mListeningState = (mListener.listen(mPort) == sf::TcpListener::Done);
        }
	}
	else
	{
		mListener.close();
		mListeningState = false;
	}
}

void Server::run()
{
    setListening(true);

    sf::Clock updateClock;

    while (isRunning())
    {
        handlePackets();
        handleConnections();
        handleDisconnections();

        if (updateClock.getElapsedTime() > mUpdateInterval)
        {
            update(mUpdateInterval);
            updateClock.restart();
        }

        sf::sleep(sf::milliseconds(100.f));
    }
}

void Server::update(sf::Time dt)
{
}

void Server::handlePackets()
{
    for (std::size_t i = 0; i < mConnectedPlayers; i++)
	{
		if (mPeers[i] != nullptr)
        {
            if (mPeers[i]->isConnected())
            {
                sf::Packet packet;
                while (mPeers[i]->poll(packet))
                {
                    handlePacket(packet,*mPeers[i]);
                    packet.clear();
                }

                if (mPeers[i]->getLastReceivePacketTime() > sf::seconds(5.f) || mPeers[i]->getLastSendPacketTime() > sf::seconds(5.f))
                {
                    mPeers[i]->remove();
                }
            }
        }
	}
}

void Server::handlePacket(sf::Packet& packet, Peer& peer)
{
    sf::Int32 packetType;
    packet >> packetType;

    for (auto itr = mPacketResponses.begin(); itr != mPacketResponses.end(); itr++)
    {
        if (itr->first == packetType && itr->second)
        {
            itr->second(packet,peer);
        }
    }
}

void Server::handleConnections()
{
    if (!mListeningState)
		return;

	if (mListener.accept(mPeers[mConnectedPlayers]->getSocketIn()) == sf::TcpListener::Done)
	{
		if (mPeers[mConnectedPlayers]->connect(*this))
        {
            onConnection(*mPeers[mConnectedPlayers]);

            mConnectedPlayers++;

            if (mConnectedPlayers >= mMaxPlayers)
                setListening(false);
            else
                mPeers.push_back(Peer::Ptr(new Peer()));
        }
	}
}

void Server::handleDisconnections()
{
    for (std::size_t i = 0; i < mPeers.size(); i++)
	{
		if (mPeers[i]->needRemove())
		{
		    onDisconnection(*mPeers[i]);

			mPeers.erase(i + mPeers.begin());

			mConnectedPlayers--;
			if (mConnectedPlayers < mMaxPlayers) // TODO : Fix wrong size
			{
				mPeers.push_back(Peer::Ptr(new Peer()));
				setListening(true);
			}

			i--;
        }
	}
}

Server& operator <<(Server& server, std::string const& v)
{
    std::string str = server.getTimeFormat() + v + "\n";
    if (server.isLogOpen())
    {
        server.getLogStream() << str;
    }
    std::cout << str;
    return server;
}
