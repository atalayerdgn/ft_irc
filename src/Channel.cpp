#include "../include/Channel.hpp"

Channel::Channel()
{
}

Channel::~Channel()
{
}


Channel::Channel(const std::string& ChannelName, const std::string& key, Client* creator)
{
	this->ChannelName = ChannelName;
	this->key = key;
	this->ChannelOwner = creator;
	this->limit = 1000;
	this->noExternalMessages = false;
	this->mode = false;
	this->topic = "No Topic is set";
	this->topicTime = "";
	this->privateChannel = false;
}

Channel& Channel::operator=(const Channel& copy)
{
	this->ChannelName = copy.ChannelName;
	this->ChannelClients = copy.ChannelClients;
	this->ChannelOwner = copy.ChannelOwner;
	this->key = copy.key;
	this->limit = copy.limit;
	this->noExternalMessages = copy.noExternalMessages;
	this->mode = copy.mode;
	this->topic = copy.topic;
	this->privateChannel = copy.privateChannel;
	this->topicTime = copy.topicTime;
	return *this;
}

void Channel::setTopic(std::string topic, Client* user)
{
	std::string top_ = topic;
	if (user != ChannelOwner)
	{
		user->sendMsg(":" + user->getHostname() + " 482 " + user->getNick() + " " + ChannelName + " :You're not channel operator\r\n");
		return;
	}
	if (topic.empty())
	{
		user->sendMsg(":" + user->getHostname() + " 331 " + user->getNick() + " " + ChannelName + " :No topic is set\r\n");
		return;
	}
	this->topic.erase();
	this->topic = top_;
	//setTopicTime();
	broadcastMessage("TOPIC " + ChannelName + " :" + topic, user);
}

Channel::Channel(const Channel& copy)
{
	*this = copy;
}

void Channel::setChannelOwner(Client* owner)
{
	this->ChannelOwner = owner;
}

void Channel::setChannelKey(const std::string& key)
{
	this->key = key;
}

void Channel::setChannelLimit(int limit)
{
	this->limit = limit;
}

void Channel::setNoExternalMessages(bool mode)
{
	this->noExternalMessages = mode;
}

void Channel::setMode(bool mode)
{
	this->mode = mode;
}

std::string Channel::getChannelName() const
{
	return ChannelName;
}

void Channel::setTopicTime()
{
	time_t rawtime;
	struct tm* timeinfo;
	char buffer[80];

	time(&rawtime);
	timeinfo = localtime(&rawtime);

	strftime(buffer, 80, "%d-%m-%Y %I:%M:%S", timeinfo);
	std::string str(buffer);
	this->topicTime = str;
}


std::string Channel::getTopicTime() const
{
	return (this->topicTime);
}


Client* Channel::getCreator() const
{
	return ChannelOwner;
}

std::string Channel::getKey() const
{
	return key;
}

std::string Channel::getTopic() const
{
	return topic;
}

bool Channel::isUserInChannel(Client* user) const
{
	for (std::vector<Client*>::const_iterator it = ChannelClients.begin(); it != ChannelClients.end(); ++it)
	{
		if ((*it) == user)
		{
			return true;
		}
	}
	return false;
}

int Channel::getLimit() const
{
	return limit;
}

int Channel::getChannelClientCount() const
{
	return ChannelClients.size();
}

void Channel::setPrivateChannel(bool status)
{
	this->privateChannel = status;
}

bool Channel::isPrivateChannel() const
{
	return privateChannel;
}

std::string Channel::getChannelInfo() const
{
	std::string info;
	info += "Channel Name: " + ChannelName + "\n";
	info += "Channel Owner: " + ChannelOwner->getNick() + "\n";
	info += "Channel Key: " + key + "\n";
	info += "Channel Limit: " + std::to_string(limit) + "\n";
	info += "Channel Mode: " + std::to_string(mode) + "\n";
	info += "Channel No External Messages: " + std::to_string(noExternalMessages) + "\n";
	info += "Channel Clients: " + getExistedClients() + "\n";
	return info;
}


bool Channel::getnoExternalMessages() const
{
	return noExternalMessages;
}

std::vector<std::string> Channel::getChannelClients() const
{
	std::vector<std::string> clients;
	for (std::vector<Client*>::const_iterator it = ChannelClients.begin(); it != ChannelClients.end(); ++it)
	{
		clients.push_back((*it)->getNick());
	}
	return clients;
}

std::string Channel::getExistedClients() const
{
	std::string clients;
	for (std::vector<Client*>::const_iterator it = ChannelClients.begin(); it != ChannelClients.end(); ++it)
	{
		clients += (*it)->getNick() + " ";
	}
	return clients;
}

bool Channel::getMode() const
{
	return mode;
}

void Channel::broadcastMessage(const std::string& message) const
{
	for (std::vector<Client*>::const_iterator it = ChannelClients.begin(); it != ChannelClients.end(); ++it)
	{
		(*it)->sendMsg(message);
	}
}

void Channel::broadcastMessage(const std::string& message, Client* user) const
{
	for (std::vector<Client*>::const_iterator it = ChannelClients.begin(); it != ChannelClients.end(); ++it)
	{
		if ((*it) != user)
		{
			(*it)->sendMsg(message);
		}
	}
}

void Channel::addClient(Client* user)
{
	ChannelClients.push_back(user);
}

void Channel::removeClient(Client* user)
{
	for (std::vector<Client*>::iterator it = ChannelClients.begin(); it != ChannelClients.end(); ++it)
	{
		if ((*it) == user)
		{
			ChannelClients.erase(it);
			break;
		}
	}

}

void Channel::setupChannelMode(Channel* channel, Client* client, std::string& mode, std::string& modeParams)
{
	if (!channel || !client)
	{
		write(2, "Channel or Client is null, cannot set mode.", 43);
		return;
	}
	if (mode == "+k" && (client->isModeratorStatus() == true))
	{
		channel->setChannelKey(modeParams);
		channel->broadcastMessage("MODE " + channel->getChannelName() + " +k " + client->getNick());
	}
	else if (mode == "+l" && (client->isModeratorStatus() == true))
	{
		channel->setChannelLimit(atoi(modeParams.c_str()));
		channel->broadcastMessage("MODE " + channel->getChannelName() + " +l " + client->getNick());
	}
	else if (mode == "+o" && (client->isModeratorStatus() == true))
	{
		Client* targetClient = Server::getInstance()->getClient(modeParams);
		if (!targetClient)
		{
			client->sendMsg(":" + client->getHostname() + " 401 " + client->getNick() + " " + modeParams + " :No such nick/channel\r\n");
			return;
		}
		targetClient->setModeratorStatus(true);
		channel->broadcastMessage("MODE " + channel->getChannelName() + " +o " + targetClient->getNick());
	}
	else if (mode == "+m" && client->isModeratorStatus() == true)
	{
		channel->setMode(true);
		channel->broadcastMessage("MODE " + channel->getChannelName() + " +m " + client->getNick());
	}
	else if (mode == "+n" && (client->isModeratorStatus() == true))
	{
		channel->setNoExternalMessages(true);
		channel->broadcastMessage("MODE " + channel->getChannelName() + " +n " + client->getNick());
	}
	else if (mode == "+p" && (client->isModeratorStatus() == true))
	{
		channel->setPrivateChannel(true);
		channel->broadcastMessage("MODE " + channel->getChannelName() + " +p " + client->getNick());
	}
	else
	{
		client->sendMsg(":" + client->getHostname() + " 501 " + client->getNick() + " :Unknown MODE flag");
	}
}

void Channel::setlowMode(Channel* channel, Client* client, std::string& mode, std::string& modeParams)
{
	if (!channel || !client)
	{
		write(2, "Channel or Client is null, cannot set mode.", 43);
		return;
	}
	if (mode == "-k" && (client->isModeratorStatus() == true))
	{
		channel->setChannelKey("");
		channel->broadcastMessage("MODE " + channel->getChannelName() + " -k " + client->getNick());
	}
	else if (mode == "-l" && (client->isModeratorStatus() == true))
	{
		channel->setChannelLimit(1000);
		channel->broadcastMessage("MODE " + channel->getChannelName() + " -l " + client->getNick());
	}
	else if (mode == "-n" && (client->isModeratorStatus() == true))
	{
		channel->setNoExternalMessages(false);
		channel->broadcastMessage("MODE " + channel->getChannelName() + " -n " + client->getNick());
	}
	else if (mode == "-o" && (client->isModeratorStatus() == true))
	{
		Client* targetClient = Server::getInstance()->getClient(modeParams);
		if (!targetClient)
		{
			client->sendMsg(":" + client->getHostname() + " 401 " + client->getNick() + " " + modeParams + " :No such nick/channel\r\n");
			return;
		}
		if (targetClient->getNick() != channel->getCreator()->getNick())
		{
			targetClient->setModeratorStatus(false);
			channel->broadcastMessage("MODE " + channel->getChannelName() + " -o " + targetClient->getNick());
		}
	}
	else if (mode == "-m" && client->isModeratorStatus() == true)
	{
		channel->setMode(false);
		channel->broadcastMessage("MODE " + channel->getChannelName() + " -m " + client->getNick());
	}
	else if (mode == "-p" && (client->isModeratorStatus() == true))
	{
		channel->setPrivateChannel(false);
		channel->broadcastMessage("MODE " + channel->getChannelName() + " -p " + client->getNick());
	}
	else
		client->sendMsg(":" + client->getHostname() + " 501 " + client->getNick() + " :Unknown MODE flag");
}
