#include "../include/Server.hpp"

void Server::setSrvPass(const std::string& pass)
{
	this->ServPass = pass;
}
std::string Server::getSrvPass() const
{
	return this->ServPass;
}

std::string Server::getServName() const
{
	return this->ServName;
}

bool Server::verifyPass(const std::string& pass)
{
	if (this->ServPass == pass)
		return true;
	return false;
}

void Server::signalHandler(int signum)
{
	Server::getInstance()->ShutDownServer();
	exit(signum);
}

void Server::ShutDownServer()
{
	std::string out = "Server shutting down...\n";
	write(STDOUT_FILENO, out.c_str(), out.size());

	for (std::map<int, Client*>::iterator it = Clients.begin(); it != Clients.end(); it++)
	{
		Client* client = it->second;
		if (Channels.size() > 0)
		{
			for (std::map<std::string, Channel*>::iterator it = Channels.begin(); it != Channels.end(); it++)
			{
				Channel* channel = it->second;
				if (channel != NULL)
				{
					channel->removeClient(client);
				}
			}

		}
		if (client != NULL && Channels.size() == 0)
		{
			client->sendMsg("Server shutting down...");
			removeClientsFromChannels(client);
			close(it->first);
			delete client;
		}
	}
	if (ServSock != -1)
	{
		close(ServSock);
		ServSock = -1;
	}
	if (bot != NULL)
	{
		delete bot;
		bot = NULL;
	}

	system("leaks -q ircserv");
	std::string outmsg = "Server has been shut down.\n";
	write(STDOUT_FILENO, outmsg.c_str(), outmsg.size());
}


