#include "../include/CommandHandler.hpp"
#include "../include/Output.hpp"
#include "../include/GameServer.hpp"

int main()
{
    Output out;
    GameServer server(&out);
    CommandHandler command(&server);
    std::string line;

    server.start();

    while (server.isRunning())
    {
        line.clear();
        std::getline(std::cin, line);
        command.handle(line);
    }

    return EXIT_SUCCESS;
}
