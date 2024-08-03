#include "../include/Client.hpp"

// -------------UTİLS PART-------------

Client::Client()
{}

Client::Client(int clisock, int cliport, const std::string& host, const std::string& serv)
:cliSockFd(clisock),cliPort(cliport),hostname(host),servname(serv)
{
	this->status = CLIENT_CONNECTED;
	this->isPass = false;
	this->UserAuthentication = false;
	this->Channels = std::vector<Channel*>();
	this->isModerator = false;
}


Client::~Client()
{
	for (std::vector<Channel*>::iterator it = Channels.begin(); it != Channels.end(); ++it)
	{
		(*it)->removeClient(this);
	}
}


Client& Client::operator=(const Client& copy)
{
	this->cliSockFd = copy.cliSockFd;
	this->cliPort = copy.cliPort;
	this->nick = copy.nick;
	this->username = copy.username;
	this->realname = copy.realname;
	this->hostname = copy.hostname;
	this->servname = copy.servname;
	this->status = copy.status;
	this->Channels = copy.Channels;
	this->UserAuthentication = copy.UserAuthentication;
	this->isPass = copy.isPass;
	this->isModerator = copy.isModerator;
	return *this;
}

Client::Client(const Client& copy)
{
	*this = copy;
}

bool Client::isValidName(const std::string& name) const
{
	const int MAX_LENGTH = 15;
	if (name.length() > MAX_LENGTH) {
		return false;
	}
	for (std::string::const_iterator it = name.begin(); it != name.end(); ++it) {
		if (!isalnum(*it)) {
			return false;
		}
	}
	return true;
}

bool Client::isSockOpen() const
{
	return this->cliSockFd != -1;
}




// -------------GETTERS-------------


std::string Client::getNick() const
{
	return this->nick;
}

std::string Client::getUsername() const
{
	return this->username;
}

std::string Client::getRealname() const
{
	return this->realname;
}

std::string Client::getHostname() const
{
	return this->hostname;
}

std::string Client::getPrefix() const
{
	std::string username = this->getUsername().empty() ? "*" : this->getUsername();
	std::string hostname = this->getHostname().empty() ? "*" : this->getHostname();

	return this->getNick() + "!" + username + "@" + hostname;	
}

std::string& Client::getBuffer()
{
	return this->buff;
}

std::vector<Channel*> Client::getChannels() const
{
	return this->Channels;
}

int Client::getCliSockFd() const
{
	return this->cliSockFd;
}

int Client::getCliPort() const
{
	return this->cliPort;
}

bool Client::getIsPass() const
{
	return this->isPass;
}

bool Client::getValidName(const std::string& nick) const
{
	return isValidName(nick);
}

bool Client::isModeratorStatus() const
{
	return this->isModerator;
}

bool Client::isUserAuthenticated() const
{
	return this->UserAuthentication;
}

bool Client::isUserRegistered() const
{
	return status == CLIENT_REGISTERED;
}

// -------------SETTERS-------------


void Client::appendBuffer(const std::string& str)
{
	this->buff.append(str);
}

void Client::clearBuffer()
{
	this->buff.clear();
}

void Client::setPass(bool pass)
{
	this->isPass = pass;
}

void Client::setNick(const std::string& nick)
{
	if (this->isValidName(nick)) {
		this->nick = nick;
	}
	else {
		std::string error = "ERROR : Invalid nickname";
		write(2, error.c_str(), error.length());
	}
}

void Client::setUsername(const std::string& username)
{
	if (this->isValidName(username)) {
		this->username = username;
	}
	else {
		std::string error = "ERROR : Invalid username";
		write(2, error.c_str(), error.length());
	}
}

void Client::setRealname(const std::string& realname)
{
	this->realname = realname;
}

void Client::setStatus(ClientStatus status)
{
	this->status = status;
}

void Client::setChannel(Channel* channel)
{
	this->Channels.push_back(channel);
}

void Client::setModeratorStatus(bool status)
{
	this->isModerator = status;
}

void Client::setUserAuthenticated(bool status)
{
	this->UserAuthentication = status;
}

void Client::sendMsg(const std::string& msg) const
{
	std::string message = msg + "\r\n";
	if (send(this->cliSockFd, message.c_str(), message.length(), 0) == -1) {
		ErrorLogger(FAILED_SOCKET_SEND, __FILE__, __LINE__);
	}
}

void Client::sendReply(const std::string& msg) const
{
	sendMsg(getPrefix() + " " + msg);	
}

void Client::leave()
{
	this->status = CLIENT_DISCONNECTED;
}
// -------------ACTİONS------------- 

void Client::WelcomeMessage()
{
	if (status != CLIENT_CONNECTED || this->nick.empty() || this->username.empty() || this->realname.empty())
	{
		sendReply("ERROR :Closing Link: " + this->hostname + " (Invalid nickname or username)");
		return;
	}

	status = CLIENT_REGISTERED;

	sendReply(WELCOME_MESSAGE(servname,nick));

	std::ostringstream oss;
	oss << this->hostname << ":" << this->cliPort << "is now your displayed host " << this->nick << ".";
	log(oss.str());
}

void Client::joinChannel(Channel* channel)
{
	channel->addClient(this);
	this->Channels.push_back(channel);
	std::string nickList;
	std::vector<std::string> nicks = channel->getChannelClients();
	for (std::vector<std::string>::iterator it = nicks.begin(); it != nicks.end(); ++it)
	{
		nickList += *it + " ";
	}
	sendReply(RPL_NAMREPLY(getPrefix(), channel->getChannelName(), nickList));
	channel->broadcastMessage(RPL_JOIN(getPrefix(), channel->getChannelName()));
	std::string msg = this->nick + " " + " has joined to the channel " + channel->getChannelName();
	log(msg);
}

void Client::removeChannel(Channel* channel)
{
	std::vector<Channel*>::iterator it = std::find(Channels.begin(), Channels.end(), channel);
	if (it != Channels.end())
		Channels.erase(it);
}

