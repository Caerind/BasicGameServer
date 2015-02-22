#include "../include/CommandHandler.hpp"

CommandHandler::CommandHandler(Server* server)
: mServer(nullptr)
{
    mServer = server;
}

CommandHandler::Arguments CommandHandler::splitArguments(std::string const& command)
{
    Arguments args;
    std::stringstream ss(command);
    std::string arg;
    while (std::getline(ss,arg,' '))
    {
        args.push_back(arg);
    }
    return args;
}

void CommandHandler::handle(std::string const& command)
{
    Arguments args = splitArguments(command);

    // Return if no arguments
    if (args.size() == 0)
    {
        return;
    }

    if (args[0] == "stop")
        handleStop(args);
    else if (args[0] == "list")
        handleList(args);
    else if (args[0] == "connected")
        handleConnected(args);
    else if (args[0] == "say")
        handleSay(args);
    else if (args[0] == "help")
        handleHelp(args);
}

void CommandHandler::handleStop(Arguments args)
{
    if (mServer != nullptr)
        mServer->stop();
}

void CommandHandler::handleList(Arguments args)
{
    if (mServer != nullptr)
        mServer->list();
}

void CommandHandler::handleConnected(Arguments args)
{
    if (mServer != nullptr)
        mServer->connected();
}

void CommandHandler::handleSay(Arguments args)
{
    std::string str;
    for (unsigned int i = 1; i < args.size(); i++)
    {
        str += args[i];
        if (i != args.size()-1)
        {
            str += " ";
        }
    }
    if (mServer != nullptr)
    {
        Output* out = mServer->getOutput();
        if (out != nullptr)
        {
            sf::Packet packet;
            Message msg;
            msg = out->write("",str);
            msg.serialize(packet);
            mServer->sendToAll(packet);
        }
    }
}

void CommandHandler::handleHelp(Arguments args)
{
    if (mServer != nullptr)
    {
        Output* out = mServer->getOutput();
        if (out != nullptr)
        {
            out->write("","help : See the list of commands");
            out->write("","stop : Stop the server");
            out->write("","list : See the list of players");
            out->write("","connected : See the number of connected players");
            out->write("","say : Say something as Server");
        }
    }
}
