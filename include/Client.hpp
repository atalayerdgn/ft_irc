#ifndef CLIENT_HPP
#define CLIENT_HPP


#include <sys/socket.h>
#include <algorithm>
#include <string>
#include <vector>
#include <iostream>
#include "Server.hpp"
#include <unistd.h>
#include "Utils.hpp"
#include "Channel.hpp"
#include <sstream>
#include "Commands.hpp"

class Server;
class Channel;

enum ClientStatus
{
	CLIENT_HANDSHAKING,
	CLIENT_CONNECTED,
	CLIENT_REGISTERED,
	CLIENT_DISCONNECTED
};


class Client
{
    public:
    Client();
    Client& operator=(const Client& copy);
    Client(const Client& copy);
    Client(int clisock, int cliport, const std::string& host, const std::string& serv);
    ~Client();

    // -------------UTİLS PART-------------

    bool isValidName(const std::string& name) const;
    bool isSockOpen() const;
    bool isModeratorStatus() const;
    bool isUserAuthenticated() const;
    bool isUserRegistered() const;

    // -------------GETTERS-------------
    int getCliSockFd() const;
    int getCliPort() const;
    bool getIsPass() const;
    bool getValidName(const std::string&) const;
    std::string getNick() const;
    std::string getUsername() const;
    std::string getRealname() const;
    std::string getHostname() const;
    std::string getPrefix() const;
    std::string& getBuffer();
    std::vector<Channel*> getChannels() const;

    // -------------SETTERS-------------

    void appendBuffer(const std::string& str);
    void clearBuffer();
    void setPass(bool pass);
    void setNick(const std::string& nick);
    void setUsername(const std::string& username);
    void setRealname(const std::string& realname);
    void setStatus(ClientStatus status);
    void setChannel(Channel* channel);
    void setModeratorStatus(bool status);
    void setUserAuthenticated(bool status);

    // -------------ACTİONS-------------

    void WelcomeMessage();
    void sendMsg(const std::string& msg) const;
    void sendReply(const std::string& msg) const;

    void leave();
    void joinChannel(Channel* channel);
    void removeChannel(Channel* channel);


    private:
    int cliSockFd;
    int cliPort;
    std::string nick;
    std::string username;
    std::string realname;
    std::string hostname;
    std::string servname;
    std::string buff;

    ClientStatus status;
    std::vector<Channel*> Channels;
    bool UserAuthentication;
    bool isPass;
    bool isModerator;

};

#endif
