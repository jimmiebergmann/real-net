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
        if(peer->State() != Net::Peer::Handshaking)
        {
            throw std::exception("Peer is not handshaking.");
        }

       /* if(peers.size())
        {
            auto otherPeer = peers.begin();
            (*otherPeer)->Disconnect();
            peers.erase(otherPeer);
        }*/

        if(peers.find(peer) != peers.end())
        {
            std::cout << "ADDING DUPLICATE OF PEER." << std::endl;
        }

        peers.insert(peer);



        std::cout << "Peer is trying to connect: " << peer->Id() << std::endl;



        PrintPeers(peers);

        return true;
    });
    server.SetOnPeerConnect([&peers](std::shared_ptr<Net::Peer> peer)
    {
        if(peer->State() != Net::Peer::Connected)
        {
            throw std::exception("Peer is not connected.");
        }

        std::cout << "Peer Connected and disconnected by server: " << peer->Id() << std::endl;

        peer->Kick();

        PrintPeers(peers);

        return true;
    });
    server.SetOnPeerDisconnect([&peers](std::shared_ptr<Net::Peer> peer, const Net::Peer::eReason reason)
    {
        if(peer->State() != Net::Peer::Disconnected)
        {
            throw std::exception("Peer is not disconnected.");
        }

        peers.erase(peer);

        std::cout << "Peer Disconnected: " << peer->Id() << " ,Reason: " << reason << "  ,Peer count: " << peers.size() << std::endl;

        /*if(reason == Net::Peer::Kicked)
        {
        }
        else if(reason == Net::Peer::Banned)
        {
        }
        else if(reason == Net::Peer::Left)
        {
        }
        else if(reason == Net::Peer::Timeout)
        {
        }
        else if(reason == Net::Peer::InvalidPacket)
        {
        }*/

        //PrintPeers(peers);

        return true;
    });

    // Start server.
    server.Host(12345, 100);

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
