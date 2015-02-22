#ifndef LOG_HPP
#define LOG_HPP

#include <fstream>
#include <string>

class Log
{
    public:
        Log();

        bool open(std::string const& filename);

        bool write(std::string const& line);

        bool isOpen() const;

    protected:
        std::ofstream mFile;
};

#endif // LOG_HPP
