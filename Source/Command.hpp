#ifndef COMMAND_HPP
#define COMMAND_HPP

#include <functional>
#include <sstream>
#include <string>
#include <vector>

class Command
{
    public:
        typedef std::function<std::string(const std::string&)> Function;

        Command();
        Command(std::string const& name, Function func, bool adminOnly = true, int permissionLevel = 0);

        std::string execute(std::string const& args);

        bool isAdminOnly() const;
        int getPermissionLevel() const;

        static std::vector<std::string> getCommandName(std::string const& command);
        static std::vector<std::string> splitArguments(std::string const& arguments);

    protected:
        std::string mName;
        Function mFunction;
        bool mAdminOnly;
        int mPermissionLevel;
};

#endif // COMMAND_HPP
