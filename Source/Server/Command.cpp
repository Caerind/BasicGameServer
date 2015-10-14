#include "Command.hpp"

Command::Command()
{
}

Command::Command(std::string const& name, Command::Function func, bool adminOnly)
: mName(name)
, mFunction(func)
, mAdminOnly(adminOnly)
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

void Command::setName(std::string const& name)
{
    mName = name;
}

void Command::setFunction(Command::Function func)
{
    mFunction = func;
}

void Command::setAdminOnly(bool adminOnly)
{
    mAdminOnly = adminOnly;
}

std::string Command::getName() const
{
    return mName;
}

bool Command::isAdminOnly() const
{
    return mAdminOnly;
}
