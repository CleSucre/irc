#ifndef BOT_HPP
	#define BOT_HPP

#include <cstdlib>
#include <string>
#include <ctime>
#include <vector>
#include <iostream>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>
#include <cstdlib>

#include "Client.hpp"
#include "Channel.hpp"



class Bot
{
	private:
		int _sock;
		const char *_nick;
		size_t _current_channel;
		size_t _channel_known;
		Client *_client;
		Channel *_channel;
		std::vector<std::pair<std::string, size_t> > _flood_timer;
		std::vector<std::pair<std::string, size_t> > _channel_list;
        std::vector<std::pair<size_t, size_t> > _client_by_channel; 

	public:
		Bot();
		~Bot();
		void server_authentification();
		int socket_creation(int argc, char* argv[]);
		void communication_loop();
		void list_channels();

		
};

#endif