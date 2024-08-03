#include <sys/event.h>
#include "../include/Server.hpp"
#define MAX_EVENTS 1024
Server* Server::ins = NULL;
Server::Server(int servsockfamily, int servsockprotocol, int servport, std::string servname)
:sockfamily(servsockfamily),protocol(servsockprotocol),port(servport)
{
    this->ServName = servname;
    this->ServPass = "";
    this->bot = NULL;
    this->ServSock = -1;
    signal(SIGINT, signalHandler);
    Server::setInstance(this);
    bzero(&servAddr,sizeof(servAddr));
}

void Server::DestructClient(std::map<int , Client*> clients)
{
    for (std::map<int, Client*>::iterator it = clients.begin(); it != clients.end() ; it++)
    {
        delete it->second;
    }
    clients.clear();
}

Server::~Server()
{
    DestructClient(this->Clients);
    DestructChannel(this->Channels);
    DestructBot(this->bot);
    delete Server::ins;
    Server::ins = NULL;
}

void Server::createSocket()
{
    int reuse;

    reuse = 1;
    this->ServSock = socket(this->sockfamily,this->protocol, 0);
    if (this->ServSock == -1)
        ErrorLogger(FAILED_SOCKET, __FILE__,__LINE__);
    if (fcntl(this->ServSock, F_SETFL, O_NONBLOCK) == -1)
        return (close(ServSock),ErrorLogger(FAILED_SOCKET_NONBLOCKING,__FILE__, __LINE__));
    if (setsockopt(this->ServSock, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(int)) == -1)
        return (close(ServSock),ErrorLogger(FAILED_SOCKET_OPTIONS,__FILE__,__LINE__));
}

void Server::InitSocket()
{
    if (sockfamily == AF_INET)
    {
        this->servAddr.sin_addr.s_addr = INADDR_ANY;
        this->servAddr.sin_family = this->sockfamily;
        this->servAddr.sin_port = htons(this->port);
    }
    else
        return (close(ServSock),ErrorLogger(FAILED_SOCKET_DOMAIN,__FILE__,__LINE__));
}

void Server::BindSocket()
{
    if (::bind(this->ServSock,reinterpret_cast<struct sockaddr*>(&this->servAddr),sizeof(this->servAddr)) == -1)
        return (close(ServSock),ErrorLogger(FAILED_SOCKET_BIND,__FILE__,__LINE__),exit(1));
}

void Server::ListenSocket()
{
    if (listen(this->ServSock, BACKLOG_SIZE) == -1)
        return (close(this->ServSock),ErrorLogger(FAILED_SOCKET_LISTEN,__FILE__,__LINE__));
}

void Server::InitCliSock(int *clifd)
{
    if (fcntl(*clifd, F_SETFL, O_NONBLOCK) == -1)
    {
        close(*clifd);
        ErrorLogger(FAILED_SOCKET_NONBLOCKING, __FILE__, __LINE__);
    }
    int reuse = 1;
    if (setsockopt(*clifd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(int)) == -1)
    {
        close(*clifd);
        ErrorLogger(FAILED_SOCKET_OPTIONS, __FILE__, __LINE__);
    }
}

int Server::AcceptSocket()
{
    struct sockaddr_storage cliAddr;
    socklen_t cliAddr_len = sizeof(cliAddr);
    int cliSock = accept(this->ServSock, (struct sockaddr*)&cliAddr, &cliAddr_len);
    if (cliSock < 0)
    {
        if (errno == EAGAIN || errno == EWOULDBLOCK)
        {}
        else
            ErrorLogger(FAILED_SOCKET_ACCEPT, __FILE__, __LINE__, true);
    }
    InitCliSock(&cliSock);

    char host[NI_MAXHOST];
    if (inet_ntop(AF_INET, &(((struct sockaddr_in *)&cliAddr)->sin_addr), host, sizeof(host)) == NULL)
    {
        ErrorLogger(FAILED_SOCKET_GETADDRINFO, __FILE__, __LINE__, true);
    }
    Client *client = NULL;
    client = new Client(cliSock, ntohs(((struct sockaddr_in*)&cliAddr)->sin_port), host, this->ServName);
    Clients.insert(std::pair<int, Client*>(cliSock, client));
    std::ostringstream messageStream;
    messageStream << "New Client: has connected.";
    client->sendMsg("Connected to Server");
    client->sendMsg("Enter the server password using /PASS");

    log(messageStream.str());
    struct kevent evSet;
    EV_SET(&evSet, cliSock, EVFILT_READ, EV_ADD, 0, 0, NULL);
    kevent(this->kq, &evSet, 1, NULL, 0, NULL);
    EV_SET(&evSet, cliSock, EVFILT_WRITE, EV_ADD, 0, 0, NULL);
    kevent(this->kq, &evSet, 1, NULL, 0, NULL);

    return cliSock;
}

void Server::RunServer()
{
    createSocket();
    InitSocket();
    BindSocket();
    ListenSocket();
    signal(SIGINT, signalHandler);
    try
    {
        this->bot = new Bot("localhost", port, ServPass);
    }
    catch (const std::exception &e)
    {
        delete bot;
        bot = NULL;
        write(STDOUT_FILENO, e.what(), strlen(e.what()));
    }

    this->kq = kqueue();
    if (this->kq == -1)
    {
        ErrorLogger("Failed to create kqueue", __FILE__, __LINE__);
        return;
    }

    struct kevent evSet;
    struct kevent evList[MAX_EVENTS];

    EV_SET(&evSet, ServSock, EVFILT_READ, EV_ADD, 0, 0, NULL);
    if (kevent(kq, &evSet, 1, NULL, 0, NULL) == -1)
    {
        ErrorLogger("Failed to add event to kqueue", __FILE__, __LINE__);
        close(kq);
        return;
    }

    EV_SET(&evSet, bot->getSocket(), EVFILT_READ, EV_ADD, 0, 0, NULL);
    if (kevent(kq, &evSet, 1, NULL, 0, NULL) == -1)
    {
        ErrorLogger("Failed to add event to kqueue", __FILE__, __LINE__);
        close(kq);
        return;
    }

    while (true)
    {
        int nev = kevent(kq, NULL, 0, evList, MAX_EVENTS, NULL);
        if (nev == -1)
        {
            ErrorLogger("kevent error", __FILE__, __LINE__);
            break;
        }

        for (int i = 0; i < nev; i++)
        {
            if (evList[i].flags & EV_ERROR)
            {
                ErrorLogger("kevent error", __FILE__, __LINE__);
                continue;
            }

            if (static_cast<int>(evList[i].ident) == ServSock)
            {
                if (evList[i].filter == EVFILT_READ)
                {
                    evList[i].ident = AcceptSocket();
                }
            }
            else if (evList[i].ident == bot->getSocket())
            {
                if (evList[i].filter == EVFILT_READ)
                {
                    bot->listen();
                }
            }
            else
            {
                int clientSock = evList[i].ident;
                if (Clients.find(clientSock) != Clients.end())
                {
                    if (evList[i].filter == EVFILT_READ)
                    {
                        ClientHandler(clientSock);
                    }
                }
            }
        }
    }
    close(kq);
}
