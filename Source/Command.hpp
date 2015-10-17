#ifndef COMMAND_HPP
#define COMMAND_HPP

// Standards Libs
#include <functional>
#include <sstream>
#include <string>
#include <vector>

class Command
{
    public:
        typedef std::function<void(const std::string&)> Function;

        Command();
        Command(std::string const& name, Function func, bool adminOnly = true);

        std::string getName() const;
        void execute(std::string const& args);
        bool isAdminOnly() const;

        static std::vector<std::string> getCommandName(std::string const& command);
        static std::vector<std::string> splitArguments(std::string const& arguments);

    protected:
        std::string mName;
        Function mFunction;
        bool mAdminOnly;
};

#endif // COMMAND_HPP
