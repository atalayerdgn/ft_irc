#include "../include/Server.hpp"


void Server::DestructChannel(std::map<std::string , Channel*> channels)
{
    for (std::map<std::string, Channel*>::iterator it = channels.begin(); it != channels.end() ; it++)
    {
        delete it->second;
    }
    channels.clear();
}

void Server::insertChannel(Channel* channel)
{
	Channels.insert(std::make_pair(channel->getChannelName(), channel));
}

Channel* Server::getChannel(std::string& name)
{
	for (std::map<std::string, Channel*>::iterator it = Channels.begin(); it != Channels.end(); it++)
	{
		if (it->second->getChannelName() == name)
			return it->second;
	}
	return NULL;
}

void Server::deleteChannel(const std::string& name)
{
	std::map<std::string, Channel*>::iterator it = Channels.find(name);
	if (it != Channels.end())
	{
		Channel* channel = it->second;
		if (channel != NULL)
		{
			delete channel;
			Channels.erase(it);
			write(STDOUT_FILENO, "Channel deleted.\n", 18);
		}
		else
		{
			std::string message = "Channel " + name + " is already null.";
			write(STDOUT_FILENO, message.c_str(), message.size());
		}
	}
	else
	{
		std::string message = "Channel " + name + " does not exist.";
		write(STDOUT_FILENO, message.c_str(), message.size());
	}
}

bool Server::existingChannel(const std::string& name)
{
	for (std::map<std::string, Channel*>::iterator it = Channels.begin(); it != Channels.end(); it++)
	{
		if (it->second->getChannelName() == name)
			return true;
	}
	return false;
}


std::map<std::string, Channel*> Server::getChannels()
{
	return Channels;
}
