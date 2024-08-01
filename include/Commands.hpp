#ifndef COMMANDS_HPP
#define COMMANDS_HPP

#include <string>
#include <vector>
#include <sstream>
#include <cctype>
#include <unistd.h>
#include "Client.hpp"
#include "Server.hpp"
#include "Channel.hpp"
#include "Bot.hpp"
#include "Utils.hpp"

class Client;
class Server;

class CommandParser
{
	public:
		static void commandParser_(const char *command, Client* client, Server* server);
		static void handleCommand(Client* client, std::vector<std::string> args, Server* server);
	private:
		CommandParser();
		~CommandParser(){};
};

class Quit
{
	public:
	static void QuitCommand(Client* client,std::vector<std::string> args, Server* server);
	private:
		Quit(){};
		~Quit(){};
};

class User
{
	public:
	static void user(Client* client, std::vector<std::string> args, Server* server);
	private:
		User(){};
		~User(){};
};

class PrivMsg
{
	private:
		PrivMsg();
		~PrivMsg(){};
	public:
		static void privMsg( Client* client, std::vector<std::string> args, Server* srv );
};

class Join
{
	private:
		Join();
		~Join(){};
	public:
		static void join( Client* client, std::vector<std::string> args, Server* srv );
		static void joinChannel( Client* client, std::string channelName, std::vector<std::string> args, Server* srv );
};

class Part
{
	private:
		Part();
		~Part(){};
	public:
		static void part( Client* client, std::vector<std::string> args, Server* srv );
		static void partChannel( Client* client, std::string channelName, Server* srv );
};

class Kick
{
	private:
		Kick();
		~Kick(){};
	public:
		static void kick( Client* client, std::vector<std::string> args, Server* srv );
};

class Mode
{
	private:
		Mode();
		~Mode(){};
	public:
		static void mode( Client* client, std::vector<std::string> args, Server* srv );
};

class Notice
{
	private:
		Notice();
		~Notice(){};
	public:
		static void notice( Client* client, std::vector<std::string> args, Server* srv );
};

class Pass
{
	private:
		Pass();
		~Pass(){};
	public:
		static void pass( Client* client, std::vector<std::string> args, Server* srv );
};

class Nick
{
	private:
		Nick();
		~Nick(){};
	public:
		static void nick( Client* client, std::vector<std::string> args, Server* srv );
};

class Cap
{
	private:
		Cap();
		~Cap(){};
	public:
		static void cap( Client* client, std::vector<std::string> args);
};

class Time
{
	private:
		Time();
		~Time(){};
	public:
		static void time( Client* client, std::vector<std::string> args, Server* srv );
};

class List
{
	private:
		List();
		~List(){};
	public:
		static void list( Client* client, std::vector<std::string> args, Server* srv );
};


class Topic
{
	private:
		Topic();
		~Topic(){};
	public:
		static void topic( Client* client, std::vector<std::string> args, Server* srv );
};
#endif
