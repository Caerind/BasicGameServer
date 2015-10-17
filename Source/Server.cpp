#include "Server.hpp"

Server::Server(std::string const& logFile)
: mThread(&Server::run,this)
, mRunning(false)
, mListener()
, mListeningState(false)
, mConnectedPlayers(0)
, mPeers(1)
, mLog(logFile)
, mMaxPlayers(10)
, mPort(4567)
, mUpdateInterval(sf::seconds(1.f/60.f))
{
    sf::Clock clock;

    write("[Server] Loading server");

    mListener.setBlocking(false);
	mPeers[0].reset(new Peer());

    loadFromFile();

    load();

    write("[Server] Loaded in " + to_string(clock.restart().asSeconds()) + "s !");
}

Server::~Server()
{
    stop();
}

void Server::load()
{
}

void Server::start()
{
    mRunning = true;
    mThread.launch();

    write("[Server] Server started !");
}

void Server::stop()
{
    if (mRunning)
    {
        write("[Server] Stopping server");

        setListening(false);

        sf::Packet packet;
        Packet::createServerStoppedPacket(packet);
        sendToAll(packet);

        mRunning = false;

        mThread.wait();

        write("[Server] Server stopped !");

        saveAdmins();
        saveBans();
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

void Server::sendToIp(sf::Packet& packet, sf::IpAddress const& ip)
{
    for (std::size_t i = 0; i < mPeers.size(); i++)
    {
        if (mPeers[i]->isConnected() && mPeers[i]->getRemoteAddress() == ip)
        {
            mPeers[i]->send(packet);
        }
    }
}

Peer::Ptr Server::getPeer(std::string const& username)
{
    for (std::size_t i = 0; i < mPeers.size(); i++)
    {
        if (mPeers[i]->getUsername() == username)
        {
            return mPeers[i];
        }
    }
    return nullptr;
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
                itr->second.execute(args[1]);
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
                write("[Server] Unknown command, try \"help\" for list of commands");
            }
            return "Unknown command";
        }
    }
    return "";
}

void Server::write(std::string const& message)
{
    // Get Time To Format
    time_t rawtime;
    struct tm* timeinfo;
    char buffer[80];
    time(&rawtime);
    timeinfo = localtime(&rawtime);
    strftime(buffer,80,"[%x][%X] ",timeinfo);

    // Write Message
    std::string str = std::string(buffer) + message + "\n";
    std::cout << str;
    if (mLog.is_open())
    {
        mLog << str;
    }
}

void Server::onConnection(Peer& peer)
{
}

void Server::onDisconnection(Peer& peer)
{
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
		if (mPeers[i]->isConnected())
        {
            sf::Packet packet;
            while (mPeers[i]->poll(packet))
            {
                sf::Int32 packetType;
                packet >> packetType;

                auto itr = mPacketResponses.find(packetType);
                if (itr != mPacketResponses.end() && itr->second)
                {
                    itr->second(packet,peer);
                }

                packet.clear();
            }
        }
	}
}

void Server::handleConnections()
{
    if (!mListeningState)
    {
        return;
    }

	if (mListener.accept(mPeers[mConnectedPlayers]->getSocketIn()) == sf::TcpListener::Done)
	{
		if (mPeers[mConnectedPlayers]->connect(*this))
        {
            onConnection(*mPeers[mConnectedPlayers]);

            mConnectedPlayers++;

            if (mConnectedPlayers >= mMaxPlayers)
            {
                setListening(false);
            }
            else
            {
                mPeers.push_back(Peer::Ptr(new Peer()));
            }
        }
	}
}

void Server::handleDisconnections()
{
    bool removePeer = false;

    for (std::size_t i = 0; i < mConnectedPlayers; i++)
	{
		if (mPeers[i]->needRemove())
		{
		    onDisconnection(*mPeers[i]);

			mPeers.erase(i + mPeers.begin());

			mConnectedPlayers--;

			removePeer = true;

			i--;
        }
	}

	if (removePeer)
    {
        if (mConnectedPlayers < mMaxPlayers)
        {
            mPeers.push_back(Peer::Ptr(new Peer()));
            setListening(true);
        }
    }
}
