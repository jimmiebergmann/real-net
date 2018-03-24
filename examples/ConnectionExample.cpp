#include <Server.hpp>
#include <Client.hpp>
#include <set>
#include <iostream>


int RunServer()
{
    Net::Server server;

    std::set<std::shared_ptr<Net::Peer>> peers;

    // Set triggers.
    server.SetOnPeerPreConnect([&peers](std::shared_ptr<Net::Peer> peer) -> bool
    {
        if(peer->State() != Net::Peer::Handshaking)
        {
            throw Net::Exception("Peer is not handshaking.");
        }

        if(peers.find(peer) != peers.end())
        {
            throw Net::Exception("Trying to add duplicate of peer.");
        }

        /*if(peers.size())
        {
            auto otherPeer = peers.begin();
            (*otherPeer)->Kick();
            peers.erase(otherPeer);
        }*/
        peers.insert(peer);

        std::cout << "Peer is trying to connect: " << peer->Id() << std::endl;

        return true;
    });

    server.SetOnPeerConnect([&peers](std::shared_ptr<Net::Peer> peer)
    {
        if(peer->State() != Net::Peer::Connected)
        {
            throw Net::Exception("Peer is not connected.");
        }
        if(peers.find(peer) == peers.end())
        {
             throw Net::Exception("SetOnPeerConnect: Unkown peer: " + std::to_string(peer->Id()));
        }

        std::cout << "Peer Connected: " << peer->Id() << std::endl;

        return true;
    });

    server.SetOnPeerDisconnect([&peers](std::shared_ptr<Net::Peer> peer, const Net::Peer::Disconnect::eReason reason)
    {
        if(peer->State() != Net::Peer::Disconnected)
        {
            throw Net::Exception("Peer is not disconnected.");
        }
        if(peers.find(peer) == peers.end())
        {
             throw Net::Exception("SetOnPeerConnect: Unkown peer: " + std::to_string(peer->Id()));
        }

        peers.erase(peer);

        std::cout << "Peer Disconnected: " << peer->Id() << ",  Reason: " << reason << ",  Peer count: " << peers.size() << std::endl;

    });

    // Start server.
    server.Host({12345, 100, Net::Address(Net::Address::Any), Net::Seconds(5.0f), Net::Seconds(2.0f)});

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
