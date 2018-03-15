#include <Server.hpp>
#include <Client.hpp>
#include <iostream>

int RunServer()
{
    Net::Server server;

    server.Host(12345);

    std::cout << "Server is running." << std::endl;
    std::cout << "Enter any character to exit." << std::endl;
    std::cin.get();
    return 0;
}

int RunClient()
{
    return 0;
}

int main(int argc, char ** argv)
{
    if(argc <= 1)
    {
        return RunServer();
    }

    return RunClient();
}
