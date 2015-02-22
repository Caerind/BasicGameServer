#include "../include/Server.hpp"

Server::Server(Output* output)
: mOutput(output)
, mThread(&Server::run,this)
, mRunning(false)
, mListener()
, mListeningState(false)
, mSettings()
, mConnectedPlayers(0)
, mPeers(1)
{
    if (mOutput != nullptr)
    {
        mOutput->init(this);
    }

    mMaxPlayers = mSettings.getInt("maxplayers");
    mPort = mSettings.getInt("port");
    mClientTimeoutTime = sf::seconds(mSettings.getFloat("timeout"));
    mUpdateInterval = sf::seconds(mSettings.getFloat("update"));

    mListener.setBlocking(false);
	mPeers[0].reset(new Peer());
}

void Server::start()
{
    if (mOutput != nullptr)
        mOutput->write("","Starting server...");

    mRunning = true;

    mThread.launch();

    if (mOutput != nullptr)
        mOutput->write("","Server started !");
}

void Server::stop()
{
    if (mOutput != nullptr)
        mOutput->write("","Stopping server...");

    setListening(false);

    sf::Packet packet;
    packet << Server2Client::ServerStopped;
    sendToAll(packet);

    mRunning = false;

    mThread.wait();

    if (mOutput != nullptr)
        mOutput->write("","Server stopped !");
}

bool Server::isRunning() const
{
    return mRunning;
}

void Server::sendToAll(sf::Packet& packet)
{
    for (std::size_t i = 0; i < mPeers.size(); i++)
    {
        if (mPeers[i]->isReady())
        {
            mPeers[i]->send(packet);
        }
    }
}

void Server::sendToPeer(sf::Packet& packet, unsigned int peerId)
{
    for (std::size_t i = 0; i < mPeers.size(); i++)
    {
        if (mPeers[i]->isReady() && mPeers[i]->getId() == peerId)
        {
            mPeers[i]->send(packet);
        }
    }
}

Settings& Server::getSettings()
{
    return mSettings;
}

Output* Server::getOutput()
{
    return mOutput;
}

void Server::list()
{
    connected();
    for (unsigned int i = 0; i < mConnectedPlayers; i++)
    {
        if (mPeers[i]->isConnected() && mOutput != nullptr)
        {
            std::string str = " - " + mPeers[i]->getName();
            mOutput->write("",str);
        }
    }
}

void Server::connected()
{
    std::string str = "Players : " + std::to_string(mConnectedPlayers) + " / " + std::to_string(mMaxPlayers);
    if (mOutput != nullptr)
        mOutput->write("",str);
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

        sf::sleep(sf::milliseconds(100));
    }
}

void Server::update(sf::Time dt)
{

}

void Server::handlePackets()
{
    bool detectedTimeout = false;

	for (unsigned int i = 0; i < mConnectedPlayers; i++)
	{
		if (mPeers[i]->isReady())
		{
			sf::Packet packet;
			while (mPeers[i]->receive(packet) == sf::Socket::Done)
			{
                handlePacket(packet,*mPeers[i], detectedTimeout);
				packet.clear();
			}

			if (mPeers[i]->getLastPacketTime() > mClientTimeoutTime)
			{
				mPeers[i]->setTimedOut(true);
				detectedTimeout = true;
			}
		}
	}

	if (detectedTimeout)
		handleDisconnections();
}

void Server::handlePacket(sf::Packet& packet, Peer& peer, bool& detectedTimeout)
{
    sf::Int32 packetType;
    packet >> packetType;

    switch (packetType)
    {
        case Client2Server::Login:
        {
            std::string username, password;
            packet >> username >> password;

            // if ok
            // - On lui envoit tout
            // - On dit aux autres qu'il est la
            // - On l'ecrit dans la console
        } break;

        case Client2Server::Disconnect:
        {
            peer.setTimedOut(true);
            detectedTimeout = true;
        } break;

        case Client2Server::Message:
        {
            Message rMsg;
            rMsg.unserialize(packet);

            if (mOutput != nullptr)
            {
                Message msg = mOutput->write(rMsg.getEmitter(),rMsg.getContent());
                sf::Packet newPacket;
                msg.serialize(newPacket);
                sendToAll(newPacket);
            }
        } break;

        default: break;
    }
}

void Server::handleConnections()
{
    if (!mListeningState)
		return;

	if (mListener.accept(mPeers[mConnectedPlayers]->getSocket()) == sf::TcpListener::Done)
	{
		mPeers[mConnectedPlayers]->setReady(true);
		mConnectedPlayers++;

		if (mConnectedPlayers >= mMaxPlayers)
			setListening(false);
		else
			mPeers.push_back(Peer::Ptr(new Peer()));
	}
}

void Server::handleDisconnections()
{
    for (auto itr = mPeers.begin(); itr != mPeers.end(); )
	{
		if ((*itr)->hasTimedOut())
		{
            // Un joueur s'est barré dans la console
            // On envoit aussi un packet aux gens

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
