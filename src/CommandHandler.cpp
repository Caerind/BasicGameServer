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
}

void CommandHandler::handleStop(Arguments args)
{
    if (mServer != nullptr)
        mServer->stop();
}
