#include "../include/Server.hpp"

void  Server::DestructBot(Bot* bot)
{
	if (bot != NULL){
		delete bot;
	}
}
Bot* Server::getBot() const
{
	return this->bot;
}

