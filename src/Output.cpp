#include "../include/Output.hpp"
#include "../include/Server.hpp"

Output::Output()
{
}

void Output::init(Server* server)
{
    std::string logfile = "";
    if (server != nullptr)
    {
        logfile = server->getSettings().getString("logfile");
    }
    if (logfile == "")
    {
        logfile = "server.log";
    }
    if(!mLog.open(logfile))
    {
        std::cout << "Error : Log file cannot be opened" << std::endl;
    }
}

Message Output::write(std::string const& emitter, std::string const& content)
{
    Message msg;

    std::string time = getTime();

    std::string realEmitter = emitter;
    if (emitter == "")
    {
        realEmitter = "[Server]";
    }

    std::string line;
    line += "[" + time + "] ";
    line += realEmitter;
    line += " : ";
    line += content;

    std::cout << line << std::endl;
    if (mLog.isOpen())
    {
        mLog.write(line);
    }

    msg.setTime(time);
    msg.setEmitter(emitter);
    msg.setContent(content);

    return msg;
}

std::string Output::getTime()
{
    std::string actualTime;

    char hour[4];
    char minute[4];
    char second[4];

    time_t rawtime;
    time(&rawtime);
    struct tm* timeInfo;
    timeInfo = localtime(&rawtime);
    strftime(hour,5,"%H",timeInfo);
    strftime(minute,5,"%M",timeInfo);
    strftime(second,5,"%S",timeInfo);

    actualTime = std::string(hour) + ":" + std::string(minute) + ":" + std::string(second);

    return actualTime;
}
