#include "../include/Commands.hpp"



// ----------------COMMAND PARSER----------------

std::string strStrim(const std::string& str) //Space trim
{
    size_t first = str.find_first_not_of(' ');
    if (std::string::npos == first)
        return str;
    size_t last = str.find_last_not_of(' ');
    return str.substr(first, (last - first + 1));
}



std::string strTrim2(const std::string& str) //whiteSpace trim
{
    const std::string whitespace = "\t\n\r\f\v";
    size_t first = str.find_first_not_of(whitespace);
    if (std::string::npos == first)
        return str;
    size_t last = str.find_last_not_of(' ');
    return str.substr(first, (last - first + 1));
}

std::string merge_(const std::vector<std::string> &parts, const std::string &delim)
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

std::vector<std::string> split_(const std::string &str, const std::string &delim)
{
    std::vector<std::string> parts;
    size_t start = 0;
    size_t end = 0;
    while ((end = str.find(delim, start)) != std::string::npos)
    {
        parts.push_back(str.substr(start, end - start));
        start = end + delim.size();
    }
    parts.push_back(str.substr(start));
    return parts;
}

// ----------------COMMAND PARSER----------------
void CommandParser::commandParser_(const char *command, Client* client, Server* server)
{
	std::string commandString(command);
	if (commandString.empty())
	    return;
	commandString.erase(std::remove(commandString.begin(), commandString.end(), '\n'), commandString.end());

    std::vector<std::string> args = split_(commandString, " ");
    if (args.size() == 0)
        return;
    for (size_t i = 0; i < args[0].length(); ++i)
        args[0][i] = std::toupper(args[0][i]);
    handleCommand(client, args, server);
}

// ----------------HANDLE COMMAND----------------


// CAP LS
// PASS
// NICK
// USER
// JOIN
// PART
// PRIVMSG
// QUIT
// MODE
// KICK
// NOTICE
void CommandParser::handleCommand(Client* client, std::vector<std::string> args, Server* server)
{
    if (args.size() < 1)
        return (client->sendMsg(":" + client->getHostname() + "461" + client->getNick() + " Command:Not enough parameters"));
    if (args[0] == "/CAP" || args[0] == "CAP")
        Cap::cap(client, args);
    else if (args[0] == "/PASS" || args[0] == "PASS")
        Pass::pass(client, args, server);
    else if (args[0] == "/NICK" || args[0] == "NICK")
        Nick::nick(client, args, server);
    else if (args[0] == "/USER" || args[0] == "USER")
    {
        if (!client->getNick().empty())
            User::user(client, args,server);
        else
            client->sendMsg(": Firstly, you need to set a nickname");
    }
else if (client->isUserAuthenticated())
{
    if (args[0] == "/JOIN" || args[0] == "JOIN")
    {
        Join::join(client, args, server);
        if (args[0] == "/JOIN" && client->isModeratorStatus() == true)
        {
            Mode::mode(client, args, server);
            WHO::who(client, args, server);
        }
    }
    else if (args[0] == "/PART" || args[0] == "PART")
        Part::part(client, args, server);
    else if (args[0] == "/PRIVMSG" || args[0] == "PRIVMSG")
        PrivMsg::privMsg(client, args, server);
    else if (args[0] == "/QUIT" || args[0] == "QUIT")
        Quit::QuitCommand(client, args, server);
    else if (args[0] == "/MODE" || args[0] == "MODE")
        Mode::mode(client, args, server);
    else if (args[0] == "/KICK" || args[0] == "KICK")
        Kick::kick(client, args, server);
    else if (args[0] == "/NOTICE" || args[0] == "NOTICE")
        Notice::notice(client, args, server);
    else if (args[0] == "/TIME" || args[0] == "TIME")
        Time::time(client, args, server);
    else if (args[0] == "/LIST" || args[0] == "LIST")
        List::list(client, args, server);
    else if (args[0] == "/TOPIC" || args[0] == "TOPIC")
        Topic::topic(client, args, server);
    else if (args[0] == "/WHO" || args[0] == "WHO")
        WHO::who(client, args, server);
    else if (args[0] == "/USERHOST" || args[0] == "USERHOST")
        UserHost::userHost(client, args, server);
    else
        return (client->sendMsg(":" + client->getHostname() + "421" + client->getNick() + " " + args[0] + " :Unknown command"));

}
else
    client->sendMsg(args[0] + " Unknown command or invalid permission");
}
