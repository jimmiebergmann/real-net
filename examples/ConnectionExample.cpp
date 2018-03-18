#include <Server.hpp>
#include <Client.hpp>
#include <iostream>

int RunServer()
{
    Net::Server server;

    // Set triggers.
    server.SetOnPeerPreConnect([](Net::Peer & peer) -> bool
    {
        std::cout << "Peer is trying to connect: " << peer.GetId() << std::endl;

        return true;
    });
    server.SetOnPeerConnect([&server](Net::Peer & peer)
    {
        std::cout << "Peer Connected: " << peer.GetId() << ". Address: " << peer.GetAddress().Ip.AsString() << "-" << peer.GetAddress().Port << std::endl;
        std::cout << "Latency: " << peer.GetLatency().AsMicroseconds() << std::endl;

        //server.DisconnectPeer(peer.GetId());
        return true;
    });
    server.SetOnPeerDisconnect([](Net::Peer & peer)
    {
        std::cout << "Peer Disconnected: " << peer.GetId() << std::endl;

        return true;
    });

    // Start server.
    server.Host(12345, 32);

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
