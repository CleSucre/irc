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

/////// temporary includes for select
// Need to check if needed
#include <sys/types.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <sys/select.h>
/////////
#include "Client.hpp"
#include "Channel.hpp"
#include "CodeMap.hpp"

class Channel;
class Client;


class Bot
{
	private:
		int _sock;
		const char *_nick;
		size_t _current_channel;
		size_t _channel_known;
		// Client *_client;
		std::vector<Channel> _channel;
		time_t _last_check;
		static const int	check_interval = 5;

		// std::vector<std::pair<std::string, size_t> > _flood_timer;
		// std::vector<std::pair<std::string, size_t> > _channel_list;
        // std::vector<std::pair<size_t, size_t> > _client_by_channel; 

	public:
		Bot();
		~Bot();
		void server_authentification();
		int socket_creation(int argc, char* argv[]);
		void communication_loop();
		// void list_channels();
		void list_channels_handler(std::string &);
		void list_users_handler(std::string &packet);

		void list_users();
		void check_flood();
		void message_reception(std::string &packet);
		void join_channels(std::vector<std::string> &channels);

		void handle_global_data();
		void user_command();
		void operator_modification(std::string &packet);

};

size_t find_channel_index(const std::vector<Channel> &channels, const std::string &channel_name);

#endif