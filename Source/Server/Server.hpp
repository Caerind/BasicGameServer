#ifndef SERVER_HPP
#define SERVER_HPP

#include <ctime>
#include <vector>
#include <fstream>
#include <sstream>
#include <map>
#include <functional>

#include <SFML/Network.hpp>
#include <SFML/System.hpp>

#include "Peer.hpp"

class Server
{
    public:
        Server(std::string const& propertiesFile, std::string const& logFile = "");
        ~Server();

        // Server
        void start();
        void stop();
        bool isRunning() const;

        // Packet
        void sendToAll(sf::Packet& packet);
        void sendToPeer(sf::Packet& packet, int peerId);
        void sendToPeer(sf::Packet& packet, Peer& peer);

        // Commands
        void handleAdminInput();
        virtual void handleCommand(std::string const& command);
        static std::vector<std::string> splitArguments(std::string const& command);

        // Settings
        std::string getSettings(std::string const& id) const;
        void setSettings(std::string const& id, std::string const& value);
        bool loadSettings();
        bool saveSettings();
        virtual void createSettings();

        // Output
        static std::string getTimeFormat();

        // Log
        bool openLog(std::string const& logFile = "");
        bool isLogOpen() const;
        std::ofstream& getLogStream();

    protected:
        virtual void initCommands();
        virtual void initPacketResponses();

        void setListening(bool enable);

        void run();
        void update(sf::Time dt);

        void handlePackets();
        void handlePacket(sf::Packet& packet, Peer& peer, bool& detectedTimeout);

        void handleConnections();
        void handleDisconnections();

    protected:
        sf::Thread mThread;
        bool mRunning;

        sf::TcpListener mListener;
        bool mListeningState;

        std::map<std::string,std::function<void(const std::string&)>> mCommands;

        std::map<sf::Int32,std::function<void(sf::Packet&,Peer&)>> mPacketResponses;

        std::map<std::string,std::string> mSettings;
        std::string mSettingsFile;

        std::ofstream mLog;

        std::size_t mMaxPlayers;
        std::size_t mPort;
        sf::Time mClientTimeoutTime;
        sf::Time mUpdateInterval;

        std::size_t mConnectedPlayers;

        std::vector<Peer::Ptr> mPeers;
};

Server& operator<<(Server& server, std::string const& v);

#endif // SERVER_HPP
