#include "Command.hpp"

Command::Command()
{
}

Command::Command(std::string const& name, Command::Function func, bool adminOnly, int permissionLevel)
: mName(name)
, mFunction(func)
, mAdminOnly(adminOnly)
, mPermissionLevel(permissionLevel)
{
}

std::string Command::execute(std::string const& args)
{
    if (mFunction)
    {
        return mFunction(args);
    }
    return "";
}

bool Command::isAdminOnly() const
{
    return mAdminOnly;
}

int Command::getPermissionLevel() const
{
    return mPermissionLevel;
}

std::vector<std::string> Command::getCommandName(std::string const& command)
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

std::vector<std::string> Command::splitArguments(std::string const& arguments)
{
    std::vector<std::string> args;
    std::stringstream ss(arguments);
    std::string arg;
    while (std::getline(ss,arg,' '))
    {
        args.push_back(arg);
    }
    return args;
}
