#include "../include/Server.hpp"


void	Server::CommandProcess(int clientSock)
{
	std::string& buf = this->cliBuff[clientSock].getBuffer();
	size_t EOL;
	std::string cmd;

	if (buf[0] == '/')
	{
		while((EOL = buf.find('\n')) != std::string::npos)
		{
			cmd = buf.substr(0,EOL);
			buf.erase(0, EOL + 1);
			CommandParser::commandParser_(cmd.c_str(), Clients[clientSock], this);
		}
		if (!buf.find('\n'))
			buf.clear();
	}
	else
	{
		while ((EOL = buf.find("\r\n")) != std::string::npos)
		{
			cmd = buf.substr(0, EOL);
			buf.erase(0, EOL + 2);
			CommandParser::commandParser_(cmd.c_str(), Clients[clientSock], this);
		}
		buf.clear();
	}
}
void    Server::ClientHandler(int ClientSocket)
{
	const size_t	BUFFSIZE = 1024;
	char			buff[BUFFSIZE];
	memset(buff, 0, BUFFSIZE);

	int n = recv(ClientSocket, buff, BUFFSIZE - 1, 0);
	if (n > 0)
	{
		cliBuff[ClientSocket].appendBuffer(std::string(buff,n));
		std::cout << "Received: " << buff << std::endl;
		CommandProcess(ClientSocket);
	}
	else if (n == 0 || errno == ECONNRESET)
	{
		DisconnectCli(ClientSocket);
		cliBuff.erase(ClientSocket);
		return;
	}
	else
	{
		if (errno != EAGAIN && errno != EWOULDBLOCK)
		{
			std::cout << "Error in recv." << std::endl;
			close(ClientSocket);
			cliBuff.erase(ClientSocket);
		}
	}
}

Client* Server::getClient(const std::string  &nickname)
{
	for (std::map<int, Client*>::iterator it = Clients.begin(); it != Clients.end(); it++)
	{
		if (it->second->getNick() == nickname)
			return it->second;
	}
	return NULL;
}

void Server::DisconnectCli(int ClientSocket)
{
	try
	{
		std::map<int, Client*>::iterator it = Clients.find(ClientSocket);
		if (it == Clients.end())
		{
			write(STDOUT_FILENO, "Client not found.\n", 18);
			return;
		}

		removeClientsFromChannels(it->second);
		it->second->leave();

		std::ostringstream messageStreamDisc;
		messageStreamDisc << "Client " << it->second->getNick() << " has disconnected.";
		log(messageStreamDisc.str());
		close(ClientSocket);
		delete it->second;
		Clients.erase(it);
		return;
	}
	catch(const std::exception& e)
	{
		write(STDOUT_FILENO, e.what(), strlen(e.what()));
	}
	
}

void Server::removeClientsFromChannels(Client* client)
{
    if (client == NULL || !client->isSockOpen())
        return;
    std::vector<Channel*> channels = client->getChannels();
    for (std::vector<Channel*>::iterator it = channels.begin(); it != channels.end(); ++it)
    {
        Channel* channel = *it;
        if (channel != NULL)
        {
            std::string channelName = channel->getChannelName();
            std::string nic = client->getNick();
            std::string leavemsg = nic + " has left the channel " + channelName;
            log(leavemsg);
            if (channel->getChannelClientCount() == 0 && existingChannel(channel->getChannelName()))
            {
                std::string msg = "Channel " + channelName + " has been deleted.\n";
                write(STDOUT_FILENO, msg.c_str(), msg.length());
                if (channel != NULL)
                    this->deleteChannel(channelName);
            }
            if (channel->getChannelClientCount() > 0)
            {
                Client *channelClient = getClient(nic);
                if (channelClient != NULL)
                {
                    Client *owner = channel->getCreator();
                    if (owner != NULL && owner->getNick() == nic)
                        channel->setChannelOwner(NULL);
                    channel->removeClient(channelClient);
                    std::vector<std::string> nicks = channel->getChannelClients();
                    if (!nicks.empty())
                    {
                        std::string owner_ = nicks.front();
                        Client *ownerClient = getClient(owner_);
						write(STDOUT_FILENO, "Owner is: ", 10);
						write(STDOUT_FILENO, owner_.c_str(), owner_.length());
						write(STDOUT_FILENO, "\n", 1);
                        if (ownerClient != NULL)
                        {
                            if (ownerClient->getNick() == owner_)
							{
                                channel->setChannelOwner(ownerClient);
								ownerClient->setModeratorStatus(true);
							}
                        }
                    }
                    channelClient->sendReply(RPL_PART(channelClient->getPrefix(), channelName));
                }
            }
        }
    }
}



std::map<int, Client*> Server::getClients() { 
		return Clients;
}
