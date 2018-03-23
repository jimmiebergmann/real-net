#include <Server.hpp>
#include <Client.hpp>
#include <set>
#include <iostream>

void PrintPeers(std::set<std::shared_ptr<Net::Peer>> & peers)
{
    std::cout << "Peer count: " << peers.size() << std::endl;
    for(auto it = peers.begin(); it != peers.end(); it++)
    {
        std::cout << (*it)->Id() << ": Latency: " << (*it)->Latency().AsMicroseconds() << "   Connected for: " << (*it)->ConnectedTime().AsSeconds() << std::endl;
    }
    std::cout << std::endl;
}

int RunServer()
{
    Net::Server server;

    std::set<std::shared_ptr<Net::Peer>> peers;

    // Set triggers.
    server.SetOnPeerPreConnect([&peers](std::shared_ptr<Net::Peer> peer) -> bool
    {
        if(peers.size())
        {
            auto otherPeer = peers.begin();
            (*otherPeer)->Disconnect();
            peers.erase(otherPeer);
        }
        peers.insert(peer);

        std::cout << "Peer is trying to connect: " << peer->Id() << std::endl;
        PrintPeers(peers);

        return true;
    });
    server.SetOnPeerConnect([&peers](std::shared_ptr<Net::Peer> peer)
    {
        std::cout << "Peer Connected: " << peer->Id() << std::endl;
        PrintPeers(peers);

        return true;
    });
    server.SetOnPeerDisconnect([&peers](std::shared_ptr<Net::Peer> peer)
    {
        peers.erase(peer);

        std::cout << "Peer Disconnected: " << peer->Id() << std::endl;
        PrintPeers(peers);

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
