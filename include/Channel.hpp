#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include <vector>
#include <string>
#include <sstream>
#include <unistd.h>
#include "Client.hpp"
#include "Server.hpp"
#include "Commands.hpp"
#include "Bot.hpp"

class Client;


class Channel
{
	public:
		Channel();
		Channel(const Channel& copy);
		Channel& operator=(const Channel& copy);
		Channel(const std::string& ChannelName,const std::string& key,Client* creator);
		~Channel();

		// -------------GETTERS-------------

		std::string getChannelName() const;
		Client* getCreator() const;
		std::string getKey() const;
		bool isUserInChannel(Client* user) const;
		int getLimit() const;
		int getChannelClientCount() const;
		bool getnoExternalMessages() const;
		std::vector<std::string> getChannelClients() const;
		std::string getExistedClients() const;
		bool getMode() const;
		std::string getChannelInfo() const;
		std::string getTopic() const;
		bool isPrivateChannel() const;
		std::string getTopicTime() const;

		// -------------SETTERS-------------

		void setPrivateChannel(bool status);
		void setChannelOwner(Client* owner);
		void setChannelKey(const std::string& key);
		void setChannelLimit(int limit);
		void setNoExternalMessages(bool mode);
		void setMode(bool mode);
		void setTopicTime();
		void setTopic(std::string topic, Client* user);

		// -------------ACTIONS-------------

		void broadcastMessage(const std::string& message) const;
		void broadcastMessage(const std::string& message,Client* user) const;
		void addClient(Client* user);
		void removeClient(Client* user);
		void setupChannelMode(Channel* channel, Client* client, std::string& mode, std::string& modeParams);
		void setlowMode(Channel* channel, Client* client, std::string& mode, std::string& modeParams);

		private:
			std::string ChannelName;
			std::vector<Client*> ChannelClients;
			Client* ChannelOwner;
			std::string key;
			std::string topic;
			int limit;
			bool noExternalMessages;
			bool mode;
			std::string topicTime;
			bool privateChannel;
};


#endif
