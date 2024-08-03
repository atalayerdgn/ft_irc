#ifndef UTILS_HPP
#define UTILS_HPP

#include <iostream>
#include <string>
#include <cstring>
#include <exception>
#include <fstream>
#include <unistd.h>
#include <sstream>
#include <ctime>

#define GET_CURRENT_TIME time(0)
#define FORMAT_TIME(t, buffer) strftime(buffer, sizeof(buffer), "%d-%m-%Y %H:%M:%S", localtime(&t))

/* SERVER FAILED/ERROR MESSAGES */
#define FAILED_SOCKET "Failed to create socket"
#define FAILED_SOCKET_OPTIONS "Failed to set socket options"
#define FAILED_SOCKET_NONBLOCKING "Failed to set socket to non-blocking"
#define FAILED_SOCKET_BIND "Failed to bind socket"
#define FAILED_SOCKET_LISTEN "Failed to listen socket"
#define FAILED_SOCKET_ACCEPT "Failed to accept socket"
#define FAILED_SOCKET_CONNECT "Failed to connect socket"
#define FAILED_SOCKET_SEND "Failed to send socket"
#define FAILED_SOCKET_RECV "Failed to recv socket"
#define FAILED_SOCKET_CLOSE "Failed to close socket"
#define FAILED_SOCKET_GETADDRINFO "Failed to getaddrinfo socket"
#define FAILED_SOCKET_DOMAIN "Not supported domain"
#define FAILED_SOCKET_RECEIVE "Failed to receive socket"

/* SERVER DEFINE */
#define BACKLOG_SIZE 100
#define MAX_CLIENTS 100

/* CLIENT FAILED/ERROR MESSAGES */
#define ERROR_SOCKET_SEND "Error while sending a message to a client!"
	/* REPLY */
#define WELCOME_MESSAGE(source3, source) "Welcome to the " + source3 + " " + source + "!"
#define RPL_NAMREPLY(source, channel, nickList) ":" + source + " 353 " + source + " = " + channel + " :" + nickList
/* COMMAND RESPONSE */
#define RPL_QUIT(source, message) ":" + source + " QUIT :Quit: " + message
#define ERR_ALREADYREGISTERED(source) "462 " + source + " :You may not register"
#define ERR_NEEDMOREPARAMS(source, command) "461 " + source + " " + command + " :Not enough parameters"
#define ERR_NOSUCHCHANNEL(source, channel) "403 " + source + " " + channel + " :No such channel"
#define ERR_USERONCHANNEL(source, channel) "443 " + source + " " + channel + " :is already on channel"
#define ERR_BADCHANNELKEY(source, channel) "475 " + source + " " + channel + " :Cannot join channel (+k)"
#define ERR_INVITEONLYCHAN(source, channel) "473 " + source + " " + channel + " :Cannot join channel (+i)"
#define ERR_CHANNELISFULL(source, channel) "471 " + source + " " + channel + " :Cannot join channel (+l)"
#define RPL_JOIN(source, channel) ":" + source + " JOIN :" + channel
#define RPL_LIST(source, channel, userCount, topic) "322 " + source + " " + channel + " " + userCount + " :" + topic
#define RPL_TIME(source, server, time) "391 " + source + " " + server + " " + time
#define RPL_ENDOFWHO(source, mask) "315 " + source + " " + mask + " :End of /WHO list"
#define RPL_ERRONEUSNICKNAME(source, nick) "432 " + source + " " + nick + " :Erroneous nickname"
#define RPL_TOPIC(source, channel, topic, time) "332 " + source + " " + channel + " " + topic + " " + time
#define RPL_WHOREPLY(channel, user, host, nick, mode, realname) \
    "\n352 Channel: " + std::string(channel) + \
    "\nUsername: " + std::string(user) + \
    "\nHostname: " + std::string(host) + \
    "\nNickname: " + std::string(nick) + \
    "\nIs Moderated: " + std::string(mode) + \
    "\nReal Name: " + std::string(realname)
#define RPL_PART(source, channel) "PART " + source + " " + channel
#define RPL_USERHOST_1(n1) "USERHOST request for information on nicks " + n1
#define RPL_USERHOST_2(n1, n2) "USERHOST request for information on nicks " + n1 + " and " + n2
#define RPL_USERHOST_3(n1, n2, n3) "USERHOST request for information on nicks " + n1 + " ," + n2 + " and " + n3
#define RPL_USERHOST_4(n1, n2, n3, n4) "USERHOST request for information on nicks " + n1 + " ," + n2 + " ," + n3 + " and " + n4
#define RPL_USERHOST_5(n1, n2, n3, n4, n5) "USERHOST request for information on nicks " + n1 + " ," + n2 + " ," + n3 + " ," + n4 + " and " + n5
#define RPL_LISTEND(source) "323 " + source + " :End of /LIST"
#define RPL_NOTOPIC(source, channel) "331 " + source + " " + channel + " :No topic is set"
#define ERR_CHANOPRIVSNEEDED(source, channel) "482 " + source + " " + channel + " :You're not channel operator"
#define RPL_TOPICWHOTIME(source, channel, nick, time) "333\n Topic : [" + source + "]\n Channel :[" + channel + "]\n " + "Created By :[" + nick + "]\n Time : [" + time + "]"
#define RPL_ENDOFNAMES(source, channel) "366 " + source + " " + channel + " :End of /NAMES list."
#define ERR_CHANOPRIVSNEEDED(source, channel) "482 " + source + " " + channel + " :You're not channel operator"
#define ERR_FLAGNOSUCH(source, flag) "501 " + source + " " + flag + " :Unknown MODE flag"
#define ERR_USERSDONTMATCH(source) "502 " + source + " :Cant change mode for other users"
#define ERR_NOTONCHANNEL(source, channel) "442 " + source + " " + channel + " :You're not on that channel"
#define ERR_NOSUCHNICK(source, nick) "401 " + source + " " + nick + " :No such nick"
#define ERR_USERNOTINCHANNEL(source, nick, channel) "441 " + source + " " + nick + " " + channel + " :They aren't on that channel"
#define ERR_NONICKNAMEGIVEN(source) "431 " + source + " :No nickname given"
#define ERR_ERRONEUSNICKNAME(source, nick) "432 " + source + " " + nick + " :Erroneous nickname"
#define ERR_NICKNAMEINUSE(source, nick) "433 " + source + " " + nick + " :Nickname is already in use"
#define ERR_NICKCOLLISION(source, nick) "436 " + source + " " + nick + " :Nickname collision KILL"
#define ERR_INVALIDCAPCMD(source) "410 " + source + " :Invalid CAP subcommand"
#define ERR_NOTEXTTOSEND(source) "412 " + source + " :No text to send"
#define ERR_NOCANNOTSENDTOCHAN(source, channel) "404 " + source + " " + channel + " :Cannot send to channel"
#define ERR_NOKICKCHANNELOWNER(source, channel) source + " " + channel + " :Cannot kick to channel owner!"
#define ERR_NOSUCHSERVER(source, server) "402 " + source + " " + server + " :No such server"

static inline void ErrorLogger(std::string messageInfo, const char* fileInfo, int lineInfo, bool isFatal = false )
{
	std::ofstream errorLog;

	errorLog.open("error.log", std::ios::app);
	errorLog << "Error Time: " << __TIME__ << std::endl;
	errorLog << "Error in file: " << fileInfo << ":" << lineInfo << std::endl;
	errorLog << "Error info: " << std::string(strerror(errno)) << std::endl;
	errorLog << "----------------------------------------" << std::endl;
	errorLog.close();

	if (isFatal) {
		throw std::runtime_error(messageInfo);
	}
	write(STDERR_FILENO, messageInfo.c_str(), messageInfo.length());
}

static inline void log(const std::string& message) {
	char buffer[100];
	time_t currentTime = time(NULL);
	std::ofstream logFile;


	strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", localtime(&currentTime));
	std::string timeStr(buffer);

	logFile.open("log.log", std::ios::app);
	logFile << timeStr << " " << message << std::endl;
	logFile.close();


	std::ostringstream oss;
	oss << "\033[0;34m[" << timeStr << "]\033[0m " << message << std::endl;

	std::string outputStr = oss.str();
	write(STDOUT_FILENO, outputStr.c_str(), outputStr.size());
}
#endif
