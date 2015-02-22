#ifndef OUTPUT_HPP
#define OUTPUT_HPP

#include <ctime>
#include <iostream>
#include <string>

#include "Log.hpp"
#include "Message.hpp"

class Server;

class Output
{
    public:
        Output();

        void init(Server* server);

        Message write(std::string const& emitter, std::string const& content);

        std::string getTime();

    protected:
        Log mLog;
};

#endif // OUTPUT_HPP
