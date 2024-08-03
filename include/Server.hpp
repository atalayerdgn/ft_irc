#ifndef SERVER_HPP
#define SERVER_HPP

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <map>
#include <csignal>
#include <unistd.h>
#include "Bot.hpp"
#include "Utils.hpp"
#include "Client.hpp"
#include "Channel.hpp"
#include "Commands.hpp"
#include <sstream>

class Client;
class Channel;
class CommandParser;
class Bot;

class Server
{
    public:

    // -------------SERV PART-------------

    
    Server(int servsockfamily, int servsockprotocol, int servport, std::string servname);
    ~Server();
    void    RunServer();
    void    ShutDownServer();
    static void signalHandler(int signal);

    void setInstance(Server* server){ins = server;}
    static Server* getInstance(){return ins;}
    void    setSrvPass(const std::string& pass);
    std::string getSrvPass() const;
    bool verifyPass(const std::string& servPass);
    void CommandProcess(int clientSock);
    std::string getServName() const;

    // -------------CLİENT PART-------------

    std::map<int, Client*> getClients();
    void    ClientHandler(int ClientSocket);
    Client* getClient(const std::string  &nickname);
    void removeClientsFromChannels(Client* client);
    void DisconnectCli(int ClientSocket);
    void DestructClient(std::map<int , Client*>);
    void InitCliSock(int *clifd);

    // -------------CHANNEL PART-------------

    void insertChannel(Channel* channel);
    Channel* getChannel(std::string& name);
    bool existingChannel(const std::string& name);
    void deleteChannel(const std::string& name);
    void DestructChannel(std::map<std::string, Channel*>);
    std::map<std::string, Channel*> getChannels();


    // -------------BOT PART-------------

    Bot* getBot() const;
    void DestructBot(Bot*);

    // -------------SOCKET PART-------------

    void    createSocket() ;
    void    InitSocket() ;
    void    BindSocket() ;
    void    ListenSocket() ;
    int     AcceptSocket() ;
    int     kq;
    
    private:
    // -------------SERV SOCKET-------------
    int ServSock;
    // -------------SERV SOCKET PROPERTIES-------------

    const int sockfamily;
    const int protocol;
    const int port;
    std::string ServName;
    std::string ServPass;

    // -------------INSTANCE-------------
    
    static Server *ins;
    
    // -------------SOCKADDR-------------

    struct sockaddr_in servAddr;

    // -------------Containers and Arrays-------------

    std::map<int, Client> cliBuff;
    std::map<int, Client*> Clients;
    std::map<std::string, Channel*> Channels;
    Bot* bot;

};

#endif
