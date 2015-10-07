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
    if (!loadSettings())
    {
        createSettings();
        saveSettings();
    }

    openLog(logFile);

    mMaxPlayers = 10;
    mPort = 4567;
    mClientTimeoutTime = sf::seconds(5.f);
    mUpdateInterval = sf::seconds(1.f/60.f);

    initCommands();
    initPacketResponses();

    mListener.setBlocking(false);
	mPeers[0].reset(new Peer());

    *this << "[Server] Server Version 0.1";
	*this << "[Server]  - Max Players : 10";
	*this << "[Server]  - Server Port : 4567";
	// Adddress

	start();
}

Server::~Server()
{
    stop();
}

void Server::start()
{
    *this << "[Server] Starting server...";
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
    }
}

bool Server::isRunning() const
{
    return mRunning;
}

void Server::sendToAll(sf::Packet& packet)
{
    for (std::size_t i = 0; i < mPeers.size(); i++)
    {
        if (mPeers[i]->isConnected())
        {
            mPeers[i]->send(packet);
        }
    }
}

void Server::sendToPeer(sf::Packet& packet, int peerId)
{
    for (std::size_t i = 0; i < mPeers.size(); i++)
    {
        if (mPeers[i]->isConnected() && mPeers[i]->getId() == peerId)
        {
            mPeers[i]->send(packet);
        }
    }
}

void Server::sendToPeer(sf::Packet& packet, Peer& peer)
{
    if (peer.isConnected())
    {
        peer.send(packet);
    }
}

void Server::handleAdminInput()
{
    while (mRunning)
    {
        std::string command;
        std::getline(std::cin, command);
        handleCommand(command);
    }
}

void Server::handleCommand(std::string const& command)
{
    std::vector<std::string> args = splitArguments(command);
    if (args.size() == 2)
    {
        for (auto itr = mCommands.begin(); itr != mCommands.end(); itr++)
        {
            if (itr->first == args[0] && itr->second)
            {
                itr->second(args[1]);
            }
        }
    }
}

std::vector<std::string> Server::splitArguments(std::string const& command)
{
    std::vector<std::string> args;
    std::size_t i = command.find(" ");
    if (i != std::string::npos)
    {
        args.push_back(command.substr(0,i));
        args.push_back(command.substr(i+1));
    }
    else
    {
        args.push_back(command);
        args.push_back("");
    }
    return args;
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
            size_t index = 0;
            while(std::isspace(line[index]))
                index++;
            const size_t beginKeyString = index;
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
    mCommands["stop"] = [&](std::string const& args)
    {
        stop();
    };

    mCommands["help"] = [&](std::string const& args)
    {
        *this << "[Server] help : Display the list of commands";
        *this << "[Server] stop : Stop the server";
        *this << "[Server] say : Say something";
    };

    mCommands["say"] = [&](std::string const& args)
    {
        *this << "[Server] : " + args;

        Message msg;
        msg.setEmitter("[Server]");
        msg.setContent(args);
        sf::Packet packet;
        Packet::createServerMessagePacket(packet,msg);
        sendToAll(packet);
    };
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
        peer.timedOut();
    };

    mPacketResponses[Packet::Type::ClientMessage] = [&](sf::Packet& packet, Peer& peer)
    {
        Message msg;
        Packet::readClientMessagePacket(packet,msg);
        packet.clear();
        if (msg.isCommand())
        {
            std::string username = peer.getUsername();
            if (username == msg.getEmitter() && true) // TODO : Permissions for commands
            {
                handleCommand(msg.getContent().substr(1));
                msg.setContent("The command has been executed !"); // TODO : Return of the command ?
            }
            else
            {
                msg.setContent("You're not allowed to do this command !");
            }
            msg.setEmitter("");
            Packet::createServerMessagePacket(packet,msg);
            sendToPeer(packet,peer);
        }
        else
        {
            Packet::createServerMessagePacket(packet,msg);
            sendToAll(packet);

            *this << msg.getEmitter() + " : " + msg.getContent();
        }
    };
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
    bool detectedTimeout = false;

	for (std::size_t i = 0; i < mConnectedPlayers; i++)
	{
		if (mPeers[i]->isConnected())
		{
			sf::Packet packet;
			while (mPeers[i]->poll(packet))
			{
                handlePacket(packet,*mPeers[i], detectedTimeout);
				packet.clear();
			}

			/*
			if (mPeers[i]->getLastPacketTime() > mClientTimeoutTime)
			{
				mPeers[i]->timedOut();
				detectedTimeout = true;
			}
			*/
		}
	}

	if (detectedTimeout)
		handleDisconnections();
}

void Server::handlePacket(sf::Packet& packet, Peer& peer, bool& detectedTimeout)
{
    sf::Int32 packetType;
    packet >> packetType;

    for (auto itr = mPacketResponses.begin(); itr != mPacketResponses.end(); itr++)
    {
        if (itr->first == packetType && itr->second)
        {
            itr->second(packet,peer);

            if (packetType == Packet::Disconnect)
            {
                detectedTimeout = true;
            }
        }
    }
}

void Server::handleConnections()
{
    if (!mListeningState)
		return;

	if (mListener.accept(mPeers[mConnectedPlayers]->getSocketIn()) == sf::TcpListener::Done)
	{
		if (mPeers[mConnectedPlayers]->connect())
        {
            std::string username = mPeers[mConnectedPlayers]->getUsername();

            sf::Packet packet;
            Packet::createClientJoinedPacket(packet,username);
            sendToAll(packet);

            *this << "[Server] " + username + " joined the game";

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
    for (auto itr = mPeers.begin(); itr != mPeers.end(); )
	{
		if (!(*itr)->isConnected())
		{
		    std::string username = mPeers[mConnectedPlayers]->getUsername();

            sf::Packet packet;
            Packet::createClientLeftPacket(packet,username);
            sendToAll(packet);

            *this << "[Server] " + username + " left the game";

			mConnectedPlayers--;

			itr = mPeers.erase(itr);

			if (mConnectedPlayers < mMaxPlayers)
			{
				mPeers.push_back(Peer::Ptr(new Peer()));
				setListening(true);
			}
        }
		else
		{
			++itr;
		}
	}
}

Server& operator<<(Server& server, std::string const& v)
{
    std::string str = server.getTimeFormat() + v + "\n";
    if (server.isLogOpen())
    {
        server.getLogStream() << str;
    }
    std::cout << str;
    return server;
}
