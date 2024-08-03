#include "../include/Commands.hpp"
#include "../include/Client.hpp"
// ---------------CAP LS----------------

void Cap::cap(Client* client, std::vector<std::string> args)
{
	if (args.size() < 2)
		return (client->sendReply(ERR_NEEDMOREPARAMS(client->getNick(), "CAP")));
	std::string cap = args[1];

	if (cap == "LS" || cap == "ls")
		client->sendMsg("/PASS\n/USER\n/NICK\n/JOIN\n/PART\n/PRIVMSG\n/NOTICE\n/KICK\n/MODE\n/TIME\n/LIST\n/TOPIC\n/WHO\n/USERHOST\n/QUIT\n");
	else
		client->sendReply(ERR_INVALIDCAPCMD(client->getNick()));
}


// ---------------PASS----------------


void Pass::pass(Client* client, std::vector<std::string> args, Server* server)
{
	if (args.size() < 2)
		return (client->sendReply(ERR_NEEDMOREPARAMS(client->getNick(), "PASS")));
	if (client->isUserRegistered() || client->isUserAuthenticated() || client->getIsPass())
		return (client->sendReply(ERR_ALREADYREGISTERED(client->getNick())));
	std::string pass = args[1];
	if (!server->verifyPass(pass))
		return (client->sendMsg("Access denied!"));
	client->setPass(true);
	client->sendMsg("Valid password. Password accepted");
}


// ---------------NICK----------------


void Nick::nick(Client* client, std::vector<std::string> args, Server* server)
{
	if (args.size() < 2)
		return (client->sendReply(ERR_NONICKNAMEGIVEN(client->getNick())));
	std::string nick = args[1];
	if (!client->getValidName(nick))
		return (client->sendReply(ERR_ERRONEUSNICKNAME(client->getNick(), nick)));
	if (client->getNick() == nick)
		return (client->sendReply(ERR_NICKNAMEINUSE(client->getNick(), nick)));

	std::map<int, Client*> clients =  server->getClients();
	std::map<int, Client*>::iterator it = clients.begin();
	while (it != clients.end())
	{
		if (!it->second->isUserRegistered())
		{
			it++;
			continue;
		}
		if (it->second->getNick() == nick)
			return (client->sendReply(ERR_NICKNAMEINUSE(client->getNick(), nick)));
		it++;
	}
    client->sendMsg(":" + client->getNick() + " NICK " + nick);
    client->setNick(nick);
}


// ---------------USER----------------


void User::user(Client *client, std::vector<std::string> args, Server *srv)
{
    if (!client || !srv)
    {
        write(2, "Error: client or srv is null\n", 29);
        return;
    }
    if (client->isUserRegistered())
    {
        client->sendReply(ERR_ALREADYREGISTERED(client->getNick()));
        return;
    }
    if (args.size() < 3)
    {
        client->sendReply(ERR_NEEDMOREPARAMS(client->getNick(), "USER"));
        return;
    }
    if (!client->getValidName(args.at(1)) || !client->getValidName(args.at(2)))
    {
        client->sendReply(ERR_ERRONEUSNICKNAME(client->getNick(), args.at(1)));
        return;
    }
    std::string userName = args.at(1);
    std::string realName = args.at(2);

    const std::map<int, Client *> &clients = srv->getClients();
    for (std::map<int, Client *>::const_iterator it = clients.begin(); it != clients.end(); ++it)
    {
        const Client *regUser = it->second;
        if (!regUser)
        {
            continue;
        }
        if (regUser != client && regUser->getUsername() == userName)
        {
            client->sendMsg("This username is already in use");
            return;
        }
    }
    client->setUsername(userName);
    client->setRealname(realName);
    client->setStatus(CLIENT_CONNECTED);
    client->setUserAuthenticated(true);

    // Kullanıcı doğrulandıysa
    if (client->isUserAuthenticated() == true)
    {
        // Sunucudan bot al ve bot varsa hoş geldin mesajını gönder
        Bot *bot = srv->getBot();
        if (bot)
            bot->WelcomeMsg(client->getNick());
    }
}



// ---------------PRIVMSG UTİLS----------------


std::string merge_privmsg(const std::vector<std::string> &parts, const std::string &delim)
{
    std::string result;
    for (size_t i = 0; i < parts.size() ; ++i)
    {
        if (i > 0)
            result += delim;
        result += parts[i];
    }
    return result;
}

std::string strStrimprivmsg(const std::string& str)
{
    size_t first = str.find_first_not_of(' ');
    if (std::string::npos == first)
        return str;
    size_t last = str.find_last_not_of(' ');
    return str.substr(first, (last - first + 1));
}

// ---------------PRIVMSG----------------
void sendChannelMessage(Client *client, std::string channelName, std::string message, Server *srv)
{
    Channel *channel = srv->getChannel(channelName);
    if (channel == NULL)
    {
        client->sendReply(ERR_NOSUCHCHANNEL(client->getNick(), channelName));
        return;
    }
    if (channel->getMode() && !client->isModeratorStatus())
    {
        client->sendReply(ERR_NOCANNOTSENDTOCHAN(client->getNick(), channelName));
        return;
    }
    if (channel->getnoExternalMessages() && !channel->isUserInChannel(client))
    {
        client->sendReply(ERR_NOCANNOTSENDTOCHAN(client->getNick(), channelName));
        return;
    }

    // Kanal mesajını yayınla
    channel->broadcastMessage(":" + client->getPrefix() + " PRIVMSG " + channelName + " :" + message, client);
}

// Özel mesajı gönderen yardımcı fonksiyon
void sendPrivateMessage(Client *client, std::string target, std::string message, Server *srv)
{
    Client *targetClient = srv->getClient(target);
    if (targetClient == NULL)
    {
        client->sendReply(ERR_NOSUCHNICK(client->getNick(), target));
        return;
    }
    targetClient->sendMsg(":" + client->getPrefix() + " PRIVMSG " + target + " :" + message);
}

void PrivMsg::privMsg(Client *client, std::vector<std::string> commandParts, Server *srv)
{
    if (commandParts.size() < 2)
    {
        client->sendReply(ERR_NEEDMOREPARAMS(client->getNick(), "PRIVMSG"));
        return;
    }
    std::string commandString = merge_privmsg(commandParts, " ");

    size_t targetStart = commandString.find("PRIVMSG") + 8;
    size_t messageStart = commandString.find(" :", targetStart);

    if (messageStart == std::string::npos)
    {
        client->sendReply(ERR_NOTEXTTOSEND(client->getNick()));
        return;
    }

    std::string target = commandString.substr(targetStart, messageStart - targetStart);
    target = strStrimprivmsg(target);
    std::string message = commandString.substr(messageStart + 2);
    if (target.at(0) == '#')
    {
        sendChannelMessage(client, target, message, srv);
    }
    else
	{
        sendPrivateMessage(client, target, message, srv);
    }
}

// ---------------JOIN----------------


void Join::join(Client* client, std::vector<std::string> args, Server* srv)
{
	if (args.size() < 2)
		return (client->sendReply(ERR_NEEDMOREPARAMS(client->getNick(), "JOIN")));
	std::string channelName = args[1];
	if (!channelName.empty())
		joinChannel(client, channelName, args, srv);
}

void Join::joinChannel(Client* client, std::string channelName, std::vector<std::string> args, Server* srv)
{
	std::string PassChannel;

	if (channelName.empty())
	{
		client->sendReply(ERR_NOSUCHCHANNEL(client->getNick(), channelName));
		return;
	}

	if (args.size() == 2)
		PassChannel = "";
	else
		PassChannel = args[2];
	
	if (channelName.size() < 2 || channelName[0] != '#')
	   return (client->sendReply(ERR_NOSUCHCHANNEL(client->getNick(), channelName)));
	Channel *channel;
	if (srv->existingChannel(channelName))
	{
		channel = srv->getChannel(channelName);
		if (channel->isUserInChannel(client))
			return (client->sendReply(ERR_USERONCHANNEL(client->getNick(), channelName)));
		else if (channel->getKey() != PassChannel)
			return (client->sendReply(ERR_BADCHANNELKEY(client->getNick(), channelName)));
		else if (channel->getLimit() <= channel->getChannelClientCount())
			return (client->sendReply(ERR_CHANNELISFULL(client->getNick(), channelName)));
	}
	else
	{
		channel = new Channel(channelName, PassChannel, client);
		channel->setChannelOwner(client);
		client->setModeratorStatus(true);
		srv->insertChannel(channel);
		channel->setNoExternalMessages(true);
		channel->broadcastMessage("MODE " + channel->getChannelName() + " +n " + client->getNick());
	}
	client->joinChannel(channel);
}


// ---------------PART----------------


void Part::part(Client* client, std::vector<std::string> args, Server* srv)
{
	if (args.size() < 2)
		return (client->sendReply(ERR_NEEDMOREPARAMS(client->getNick(), "PART")));
	std::string channelName = args[1];

	if (channelName.find(',') == std::string::npos)
		partChannel(client, channelName, srv);
	else
	{
		std::istringstream is(channelName);
		std::string channel;
		while (getline(is, channel, ','))
			partChannel(client, channel, srv);
	}
}

void Part::partChannel(Client* client, std::string channelName, Server* srv)
{
	if (channelName.empty() || channelName[0] != '#')
		return (client->sendMsg( ":" + client->getHostname() + " 403 " + client->getNick() + " " + channelName + " :No such channel" ));
	Channel *channel = srv->getChannel(channelName);
	if (channel == NULL)
		return (client->sendMsg( ":" + client->getHostname() + " 403 " + client->getNick() + " " + channelName + " :No such channel" ));
	if (!channel->isUserInChannel(client))
		return (client->sendReply(ERR_NOTONCHANNEL(client->getNick(), channelName)));
	if (!channel->isUserInChannel(client))
		return (client->sendMsg( ":" + client->getHostname() + " 442 " + client->getNick() + " " + channelName + " :You're not on that Channel" ));
	channel->removeClient(client);
	client->removeChannel(channel);

	std::string message = ":" + client->getPrefix() + " PART " + channelName + "\n";
	channel->broadcastMessage(message, client);

	client->sendMsg("You have left the channel " + channelName);
	if (channel->getChannelClientCount() == 0 && srv->existingChannel(channelName))
	{
		std::string name = channel->getChannelName();
		std::string msg = "Channel " + name + " is empty, it will be deleted.\n";
		write(1, msg.c_str(), msg.size());
		srv->deleteChannel(name);
	}
	else if (channel->getChannelClientCount() > 0)
	{
		std::vector<std::string> nicks = channel->getChannelClients();
    	if (!nicks.empty())
    	{
    	    std::string owner_ = nicks.front();
    	    Client *ownerClient = srv->getClient(owner_);
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
	}
}

// ---------------KICK----------------


void Kick::kick( Client* client, std::vector<std::string> args, Server* srv )
{
	if (args.size() < 3)
		return (client->sendReply(ERR_NEEDMOREPARAMS(client->getNick(), "KICK")));
	std::string channelName = args[1];
	std::string target = args[2];
	std::string reason = (args[3].size() > 3) ? args[3] : "No reason given";

	if (channelName.empty() || channelName[0] != '#')
		return (client->sendReply(ERR_NOSUCHCHANNEL(client->getNick(), channelName)));
	Channel *channel = srv->getChannel(channelName);
	if (channel == NULL)
		return (client->sendReply(ERR_NOSUCHCHANNEL(client->getNick(), channelName)));
	if (!channel->isUserInChannel(client))
		return (client->sendReply(ERR_NOTONCHANNEL(client->getNick(), channelName)));
	Client* target_ = srv->getClient(target);
	if (!target_)
		return (client->sendReply(ERR_NOSUCHNICK(client->getNick(), target)));
	if (!channel->isUserInChannel(target_))
		return (client->sendReply(ERR_USERNOTINCHANNEL(client->getNick(), target_->getNick(), channelName)));
	if (channel->getCreator()->getNick() == target_->getNick())
		return (client->sendReply(ERR_NOKICKCHANNELOWNER(client->getNick(), channelName)));
	if (client->isModeratorStatus())
	{
		channel->removeClient(target_);
		target_->removeChannel(channel);
		std::string message = ":" + client->getPrefix() + " KICK " + channelName + " " + target + " :" + reason + "\n";
		channel->broadcastMessage(message, client);
		target_->sendMsg(message);
		target_->sendMsg("You have been kicked from the channel " + channelName + " by " + client->getNick() + " Reason: " + reason);

		std::string leavemsg = client->getNick() + "kicked" + target_->getNick() + "from the channel" + channelName + "Reason: " + reason;
		log(leavemsg);
	}
	else
		return(client->sendReply(ERR_CHANOPRIVSNEEDED(client->getNick(), channelName)));
	if (channel->getChannelClientCount() == 0)
	{
		std::string name = channel->getChannelName();
		std::string msg = "Channel " + name + " is empty, it will be deleted.\n";
		write(1, msg.c_str(), msg.size());
		srv->deleteChannel(name);
	}	
}


// ---------------MODE----------------



std::string strTrim2_mode(const std::string& str)
{
    const std::string whitespace = "\t\n\r\f\v";
    size_t first = str.find_first_not_of(whitespace);
    if (std::string::npos == first)
        return str;
    size_t last = str.find_last_not_of(' ');
    return str.substr(first, (last - first + 1));
}

void Mode::mode( Client* client, std::vector<std::string> args, Server* srv )
{
	if (args.size() < 2)
		return (client->sendMsg(":" + client->getHostname() + "461" + client->getNick() + " MODE :Not enough parameters"));
	std::string targ = args[1];
	std::string mode = (args.size() > 2) ? args[2] : "";
	std::string modeparam = "100";
	if (targ.empty())
		return (client->sendMsg(":" + client->getHostname() + "403" + client->getNick() + " MODE :No such channel\r\n"));
	if (targ[0] == '#')
	{
		std::string channelName = strTrim2_mode(targ);
		Channel *channel = srv->getChannel(channelName);
		if (channel == NULL)
			return (client->sendMsg(":" + client->getHostname() + "403" + client->getNick() + " MODE :No such channel\r\n"));
		if (mode.empty())
			return;
		if (mode[0] == '+')
		{
			if (args.size() > 3)
			{
				modeparam = args[3];
				channel->setupChannelMode(channel, client, mode, modeparam);
			}
			else
				channel->setupChannelMode(channel, client, mode, modeparam);
		}
		else if (mode[0] == '-')
		{
			if (args.size() > 3)
			{
				modeparam = args[3];
				channel->setlowMode(channel, client, mode, modeparam);
			}
			else
				channel->setlowMode(channel, client, mode, modeparam);
		}
		else
			client->sendMsg(":" + client->getHostname() + "501" + client->getNick() + " :Unknown MODE flag");
	}
	else
		client->sendMsg(":" + client->getHostname() + "501" + client->getNick() + " :Unknown MODE flag");
}


// ---------------NOTICE----------------

static void sendNoticeChannelMessage(Client *client, std::string channelName, std::string message, Server *srv)
{
    // Belirtilen kanalı sunucudan al
    Channel *channel = srv->getChannel(channelName);

    // Kanal bulunamazsa hata mesajı gönder
    if (channel == NULL  || !channel->isUserInChannel(client))
    {
        client->sendReply(ERR_NOSUCHCHANNEL(client->getNick(), channelName));
        return;
    }

    // Kanalın tüm kullanıcılarına NOTICE mesajını gönder
    channel->broadcastMessage(":" + client->getPrefix() + " NOTICE " + channelName + " :" + message);
}

static void sendNoticeMessage(Client *client, std::string target, std::string message, Server *srv)
{
    // Belirtilen kullanıcıyı sunucudan al
    Client *targetClient = srv->getClient(target);

    // Kullanıcı bulunamazsa hata mesajı gönder
    if (targetClient == NULL)
    {
        client->sendReply(ERR_NOSUCHNICK(client->getNick(), target));
        return;
    }

    // Kullanıcıya NOTICE mesajını gönder
    targetClient->sendMsg(":" + client->getPrefix() + " NOTICE " + target + " :" + message);
}

void Notice::notice(Client *client, std::vector<std::string> args, Server *srv)
{
	if (args.size() < 3)
		return (client->sendMsg(":" + client->getHostname() + "461" + client->getNick() + " NOTICE :Not enough parameters"));
	std::string cmdstr = merge_privmsg(args, " ");
	size_t pos = cmdstr.find("NOTICE") + 7;
	size_t message = cmdstr.find(":", pos);

	if (message == std::string::npos)
		return (client->sendMsg(":" + client->getHostname() + "412" + client->getNick() + " :No text to send"));

	std::string target = cmdstr.substr(pos, message - pos);
	std::string message_ = cmdstr.substr(message + 2);

	if (target.empty())
		return (client->sendMsg(":" + client->getHostname() + "401" + client->getNick() + " :No recipient given (PRIVMSG)"));
	if (message_.empty())
		return (client->sendMsg(":" + client->getHostname() + "412" + client->getNick() + " :No text to send"));
	target = strStrimprivmsg(target);

	if (target[0] == '#')
		sendNoticeChannelMessage(client, target, message_, srv);
	else
		sendNoticeMessage(client, target, message_, srv);
}


// ---------------QUIT----------------

void Quit::QuitCommand(Client *client, std::vector<std::string> args, Server *srv)
{
    std::string reason = args.empty() ? "Leaving from Server" : args[0];
    if (reason[0] == ':')
        reason.substr(1);
    client->sendMsg(RPL_QUIT(client->getPrefix(), reason));
    srv->removeClientsFromChannels(client);
    srv->DisconnectCli(client->getCliSockFd());
}


// ---------------TIME----------------


void Time::time(Client* client, std::vector<std::string> args, Server* srv)
{
	if (srv == NULL)
		return (client->sendReply(ERR_NOSUCHSERVER(client->getNick(), client->getHostname()))); 
	time_t rawtime;
	struct tm *timeinfo;
	::time(&rawtime);
	timeinfo = localtime(&rawtime);
	std::string time = asctime(timeinfo);
	client->sendReply(RPL_TIME(client->getNick(), srv->getServName(), time));
}


// ---------------LIST---------------- düzeltilecek

void List::list( Client* client, std::vector<std::string> args, Server* srv)
{
	if (srv == NULL)
		return (client->sendReply(ERR_NOSUCHSERVER(client->getNick(), client->getHostname())));
	if (srv->getChannels().empty())
		return (client->sendReply(RPL_LISTEND(client->getNick())));
	std::map<std::string, Channel*> channels = srv->getChannels();
	std::map<std::string, Channel*>::iterator it = channels.begin();
	while (it != channels.end())
	{
		if (it->second->isPrivateChannel() && it->second->isUserInChannel(client))
		{
			Channel *channel = it->second;
			std::string channelName = channel->getChannelName();
			std::string channelInfo = channel->getTopic();
			client->sendReply(RPL_LIST(client->getNick(), channelName, std::to_string(channel->getChannelClientCount()), channelInfo));
		}
		if (!it->second->isPrivateChannel())
		{
			Channel *channel = it->second;
			std::string channelName = channel->getChannelName();
			std::string channelInfo = channel->getTopic();
			client->sendReply(RPL_LIST(client->getNick(), channelName, std::to_string(channel->getChannelClientCount()), channelInfo));
		}
		it++;
	}
	client->sendReply(RPL_LISTEND(client->getNick()));
}

// ---------------TOPIC----------------


void Topic::topic(Client* client, std::vector<std::string> args, Server* srv) {
    if (args.size() < 2) {
        return client->sendReply(ERR_NEEDMOREPARAMS(client->getNick(), "TOPIC"));
    }

    std::string channelName = args[1];
    std::string topic = (args.size() > 2) ? args[2] : "";
	if (topic[1] == ':')
		topic = topic.substr(1, topic.size() - 1);
    if (channelName.empty() || channelName[0] != '#') {
        return client->sendReply(ERR_NOSUCHCHANNEL(client->getNick(), channelName));
    }

    Channel* channel = srv->getChannel(channelName);
    if (channel == NULL) {
        return client->sendReply(ERR_NOSUCHCHANNEL(client->getNick(), channelName));
    }

    if (!channel->isUserInChannel(client)) {
        return client->sendReply(ERR_NOTONCHANNEL(client->getNick(), channelName));
    }
    if (topic.empty() && args.size() == 2) {
        if (channel->getTopic().empty()) {
            return client->sendReply(RPL_NOTOPIC(client->getNick(), channelName));
        } else {
            client->sendReply(RPL_TOPIC(client->getNick(), channelName, channel->getTopic(), channel->getTopicTime()));
			client->sendReply(RPL_TOPICWHOTIME(channel->getTopic(), channelName, client->getNick(), channel->getTopicTime()));
            return;
        }
	}
    std::string absolutetopic = (topic[0] == ':') ? topic.substr(1) : topic.substr(1, topic.size() - 1);
    if (absolutetopic.empty()) {
        absolutetopic = "No topic is set";
    }
    channel->setTopic(absolutetopic, client);
    channel->setTopicTime();
    std::string message = ":" + client->getPrefix() + " TOPIC " + channelName + " :" + absolutetopic + "\n";
    channel->broadcastMessage(message, client);
    client->sendReply(RPL_TOPIC(client->getNick(), channelName, absolutetopic, channel->getTopicTime()));
}


void WHO::who(Client* client, std::vector<std::string> args, Server* srv)
{
    if (args.size() < 2)
        return client->sendReply(ERR_NEEDMOREPARAMS(client->getNick(), "WHO"));
    std::string channelName = args[1];
    std::string mode = (args.size() > 2 && args[2] == "o") ? args[2] : "";
    Channel* channel = NULL;

    if (channelName[0] == '#')
    {
		if (client->isModeratorStatus() == false)
			return client->sendReply(ERR_CHANOPRIVSNEEDED(client->getNick(), args[1]));
        channel = srv->getChannel(channelName);
        if (!channel)
		{
			std::string message = "Channel " + channelName + " does not exist.";
			write(STDOUT_FILENO, message.c_str(), message.size());
			return ;
		}
		std::vector<std::string> users = channel->getChannelClients();
		if (users.empty())
			return (client->sendReply(RPL_ENDOFWHO(channelName, "o")));
        for (std::vector<std::string>::iterator it = users.begin(); it != users.end(); ++it)
        {
			Client* user = srv->getClient(*it);

			if (user->isModeratorStatus() && mode == "")
			    client->sendReply(RPL_WHOREPLY(channelName, user->getUsername(), user->getHostname(), user->getNick(), "True", user->getRealname()));
			else if (user->isModeratorStatus() && mode == "o")
				client->sendReply(RPL_WHOREPLY(channelName, user->getUsername(), user->getHostname(), user->getNick(), "True", user->getRealname()));
            else if (!user->isModeratorStatus() && mode == "")
                client->sendReply(RPL_WHOREPLY(channelName, user->getUsername(), user->getHostname(), user->getNick(), "False", user->getRealname()));
        }
    }
    else if (channelName[0] == '*')
    {
        if (channelName[1] == '.')
        {
            std::string channelName_ = channelName.substr(2, channelName.size() - 1);
            std::map<int, Client*> clients = srv->getClients();
            if (clients.empty()) return;

            std::vector<std::string> users;
            for (std::map<int, Client*>::iterator it = clients.begin(); it != clients.end(); ++it)
            {
                if (strstr(it->second->getNick().c_str(), channelName_.c_str()) != NULL)
                    users.push_back(it->second->getNick());
            }
            if (users.empty()) return (client->sendReply(RPL_ERRONEUSNICKNAME(channelName, client->getNick())));

            for (std::vector<std::string>::iterator it = users.begin(); it != users.end(); ++it)
            {
                Client* user = srv->getClient(*it);
                if (!user) continue;

				if (user->isModeratorStatus() && mode == "")
				    client->sendReply(RPL_WHOREPLY(channelName_, user->getUsername(), user->getHostname(), user->getNick(), "True", user->getRealname()));
				else if (user->isModeratorStatus() && mode == "o")
					client->sendReply(RPL_WHOREPLY(channelName_, user->getUsername(), user->getHostname(), user->getNick(), "True", user->getRealname()));
                else if (!user->isModeratorStatus() && mode == "")
                    client->sendReply(RPL_WHOREPLY(channelName_, user->getUsername(), user->getHostname(), user->getNick(), "False", user->getRealname()));
            }
        }
        else
        {
            std::string channelName_ = channelName.substr(1, channelName.size() - 1);
            std::map<int, Client*> clients = srv->getClients();
            if (clients.empty()) return;

            std::vector<std::string> users;
            for (std::map<int, Client*>::iterator it = clients.begin(); it != clients.end(); ++it)
            {
                if (strstr(it->second->getNick().c_str(), channelName_.c_str()) != NULL)
                    users.push_back(it->second->getNick());
            }
            if (users.empty()) return (client->sendReply(RPL_ERRONEUSNICKNAME(channelName, client->getNick())));

            for (std::vector<std::string>::iterator it = users.begin(); it != users.end(); ++it)
            {
                Client* user = srv->getClient(*it);
                if (!user) continue;

				if (user->isModeratorStatus() && mode == "")
				    client->sendReply(RPL_WHOREPLY(channelName_, user->getUsername(), user->getHostname(), user->getNick(), "True", user->getRealname()));
				else if (user->isModeratorStatus() && mode == "o")
					client->sendReply(RPL_WHOREPLY(channelName_, user->getUsername(), user->getHostname(), user->getNick(), "True", user->getRealname()));
                else if (!user->isModeratorStatus() && mode == "")
                    client->sendReply(RPL_WHOREPLY(channelName_, user->getUsername(), user->getHostname(), user->getNick(), "False", user->getRealname()));
            }
        }
    }
    else if (channelName[channelName.size() - 1] == '*')
    {
        std::string channelName_ = channelName.substr(0, channelName.size() - 1);
        std::map<int, Client*> clients = srv->getClients();
        if (clients.empty()) return;

        std::vector<std::string> users;
        for (std::map<int, Client*>::iterator it = clients.begin(); it != clients.end(); ++it)
        {
            if (strstr(it->second->getNick().c_str(), channelName_.c_str()) != NULL)
                users.push_back(it->second->getNick());
        }
        if (users.empty()) return (client->sendReply(RPL_ERRONEUSNICKNAME(channelName, client->getNick())));

        for (std::vector<std::string>::iterator it = users.begin(); it != users.end(); ++it)
        {
            Client* user = srv->getClient(*it);
            if (!user) continue;

			if (user->isModeratorStatus() && mode == "")
			    client->sendReply(RPL_WHOREPLY(channelName_, user->getUsername(), user->getHostname(), user->getNick(), "True", user->getRealname()));
			else if (user->isModeratorStatus() && mode == "o")
				client->sendReply(RPL_WHOREPLY(channelName_, user->getUsername(), user->getHostname(), user->getNick(), "True", user->getRealname()));
            else if (!user->isModeratorStatus() && mode == "")
                client->sendReply(RPL_WHOREPLY(channelName_, user->getUsername(), user->getHostname(), user->getNick(), "False", user->getRealname()));
        }
    }
	else
		client->sendReply(RPL_ERRONEUSNICKNAME(channelName, client->getNick()));
    std::string message = ":" + client->getPrefix() + " WHO " + channelName + "\n";
    if (channel) channel->broadcastMessage(message, client);
}


void UserHost::userHost(Client* client, std::vector<std::string> args, Server* srv)
{
	if (args.size() < 2)
		return (client->sendReply(ERR_NEEDMOREPARAMS(client->getNick(), "USERHOST")));
	if (args.size() > 6)
		return (client->sendReply(ERR_NEEDMOREPARAMS(client->getNick(), "USERHOST")));
	if (args.size() == 2)
	{
		std::string nick = args[1];
		Client* user = srv->getClient(nick);
		if (!user)
			return (client->sendReply(ERR_NOSUCHNICK(client->getNick(), nick)));
		client->sendReply(RPL_USERHOST_1(user->getNick()));
	}
	if (args.size() == 3)
	{
		std::string nick = args[1];
		std::string nick2 = args[2];

		Client* user = srv->getClient(nick);
		Client* user2 = srv->getClient(nick2);
		if (!user || !user2)
			return (client->sendReply(ERR_NOSUCHNICK(client->getNick(), nick)));
		client->sendReply(RPL_USERHOST_2(user->getNick(), user2->getNick()));
	}
	if (args.size() == 4)
	{
		std::string nick = args[1];
		std::string nick2 = args[2];
		std::string nick3 = args[3];

		Client* user = srv->getClient(nick);
		Client* user2 = srv->getClient(nick2);
		Client* user3 = srv->getClient(nick3);
		if (!user || !user2 || !user3)
			return (client->sendReply(ERR_NOSUCHNICK(client->getNick(), nick)));
		client->sendReply(RPL_USERHOST_3(user->getNick(), user2->getNick(), user3->getNick()));
	}
	if (args.size() == 5)
	{
		std::string nick = args[1];
		std::string nick2 = args[2];
		std::string nick3 = args[3];
		std::string nick4 = args[4];

		Client* user = srv->getClient(nick);
		Client* user2 = srv->getClient(nick2);
		Client* user3 = srv->getClient(nick3);
		Client* user4 = srv->getClient(nick4);
		if (!user || !user2 || !user3 || !user4)
			return (client->sendReply(ERR_NOSUCHNICK(client->getNick(), nick)));
		client->sendReply(RPL_USERHOST_4(user->getNick(), user2->getNick(), user3->getNick(), user4->getNick()));
	}
	if (args.size() == 6)
	{
		std::string nick = args[1];
		std::string nick2 = args[2];
		std::string nick3 = args[3];
		std::string nick4 = args[4];
		std::string nick5 = args[5];

		Client* user = srv->getClient(nick);
		Client* user2 = srv->getClient(nick2);
		Client* user3 = srv->getClient(nick3);
		Client* user4 = srv->getClient(nick4);
		Client* user5 = srv->getClient(nick5);
		if (!user || !user2 || !user3 || !user4 || !user5)
			return (client->sendReply(ERR_NOSUCHNICK(client->getNick(), nick)));
		client->sendReply(RPL_USERHOST_5(user->getNick(), user2->getNick(), user3->getNick(), user4->getNick(), user5->getNick()));
	}
}


