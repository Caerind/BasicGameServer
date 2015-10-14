#ifndef COMMAND_HPP
#define COMMAND_HPP

#include <string>
#include <functional>

class Command
{
    public:
        typedef std::function<std::string(const std::string&)> Function;

        Command();
        Command(std::string const& name, Function func, bool adminOnly = true);

        std::string execute(std::string const& args);

        void setName(std::string const& name);
        void setFunction(Function func);
        void setAdminOnly(bool adminOnly);

        std::string getName() const;
        bool isAdminOnly() const;

    protected:
        std::string mName;
        Function mFunction;
        bool mAdminOnly;
};

#endif // COMMAND_HPP
