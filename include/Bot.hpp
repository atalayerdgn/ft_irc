#ifndef BOT_HPP
#define BOT_HPP

#include "Server.hpp"
#include "Channel.hpp"
#include "Client.hpp"
#include <string>
#include <unistd.h>


class Bot
{
	public:
		Bot(const std::string &serv, int port, const std::string &pass);
		~Bot();
		std::string getBotnick() const { return nick; }
		void connectServ();
		void sendMsg(const std::string &channel, const std::string &msg);
		void sendRegMsg(const std::string &msg);
		void listen();
		void processMessage(const std::string &msg);
		int getSocket() const;
		void WelcomeMsg(const std::string &userNick) {
			sendMsg(userNick, "Welcome! We've been waiting for you!");}
	private:
		int		sock;
		std::string	serv;
		int		port;
		std::string	botPass;
		std::string	nick;
		std::string	user;
		std::string	realname;

};


#endif
