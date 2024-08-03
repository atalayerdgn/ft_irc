#include "../include/Bot.hpp"

Bot::Bot(const std::string &serv, int port, const std::string &pass)
	: serv(serv),
	  port(port),
	  botPass(pass),
	  nick("kayraBot"),
	  user("IRC"),
	  realname("Bot")
{
	connectServ();
}

Bot::~Bot()
{
	close(sock);
}

void Bot::connectServ()
{
	struct sockaddr_in server_addr;
	struct hostent *host;

	host = gethostbyname(serv.c_str());
	if ((sock = socket(AF_INET, SOCK_STREAM, 0)) == -1)
	{
		write(STDOUT_FILENO, "Error: Bot socket creation failed.\n", 34);
		exit(1);
	}

	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(port);
	server_addr.sin_addr = *((struct in_addr *)host->h_addr);
	memset(&(server_addr.sin_zero), '\0', 8);

	if (connect(sock, (struct sockaddr *)&server_addr, sizeof(struct sockaddr)) == -1)
	{
		write(STDOUT_FILENO, "Error: Bot socket connection failed.\n", 36);
		exit(1);
	}

	if (fcntl(sock, F_SETFL, O_NONBLOCK) == -1)
	{
		write(STDOUT_FILENO, "Error: Bot socket fcntl failed.\n", 32);
		exit(1);
	}

	if (sock != -1)
	{
		sendRegMsg("PASS " + botPass + "\r\n");
		sendRegMsg("NICK " + nick + "\r\n");
		sendRegMsg("USER " + user + " 0 * :" + realname + "\r\n");
	}
}

void Bot::listen()
{
	const int BUFFER_SIZE = 1024;
	char buffer[BUFFER_SIZE];
	memset(buffer, 0, BUFFER_SIZE);
	std::string accumulated;

	int bytes_read = recv(sock, buffer, BUFFER_SIZE - 1, 0);
	if (bytes_read < 0)
	{
		std::string errrecv = "Error in recv: " + std::string(strerror(errno)) + "\n";
		write(STDOUT_FILENO, errrecv.c_str(), errrecv.length());
		return;
	}
	else if (bytes_read == 0)
	{
		write(STDOUT_FILENO, "Connection closed by peer.\n", 27);
		return;
	}
	buffer[bytes_read] = '\0';
	accumulated += buffer;

	size_t pos;
	while ((pos = accumulated.find("\r\n")) != std::string::npos)
	{
		std::string msg = accumulated.substr(0, pos);
		accumulated.erase(0, pos + 2);
		processMessage(msg);
	}
}

void Bot::processMessage(const std::string &msg)
{
	if (msg.find("PRIVMSG") != std::string::npos)
	{
		std::string senderNick = msg.substr(1, msg.find("!") - 1);
		{
			if (msg.find("hello") != std::string::npos)
				sendMsg(senderNick, "Hello " + senderNick + "!");

			if (msg.find("time") != std::string::npos || msg.find("date") != std::string::npos)
			{
				time_t rawtime;
				struct tm *timeinfo;
				time(&rawtime);
				timeinfo = localtime(&rawtime);
				sendMsg(senderNick, asctime(timeinfo));
			}

			if (msg.find("amk") != std::string::npos)
				sendMsg(senderNick, "Please be more respectful!  " + senderNick + "!");
			else if (msg.find("salak") != std::string::npos)
				sendMsg(senderNick, "Please be more respectful!  " + senderNick + "!");
			else if (msg.find("mal") != std::string::npos)
				sendMsg(senderNick, "Please be more respectful!  " + senderNick + "!");
			else if (msg.find("uglyasf") != std::string::npos)
				sendMsg(senderNick, "Please be more respectful! " + senderNick + "!");
			else if (msg.find("screw") != std::string::npos)
				sendMsg(senderNick, "Please be more respectful!  " + senderNick + "!");
			else if (msg.find("dumb") != std::string::npos)
				sendMsg(senderNick, "Please be more respectful!  " + senderNick + "!");
			else if (msg.find("stupid") != std::string::npos)
				sendMsg(senderNick, "Please be more respectful!  " + senderNick + "!");
			else if (msg.find("idiot") != std::string::npos)
				sendMsg(senderNick, "Please be more respectful!  " + senderNick + "!");
		}
	}
}
void Bot::sendMsg(const std::string &channel, const std::string &message)
{
	std::string fullMessage = "PRIVMSG " + channel + " :" + message + "\r\n";
	send(sock, fullMessage.c_str(), fullMessage.length(), 0);
}

void Bot::sendRegMsg(const std::string &message)
{
	send(sock, message.c_str(), message.length(), 0);
}

int Bot::getSocket() const
{
	return sock;
}
