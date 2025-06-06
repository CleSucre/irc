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

#include <sys/types.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <sys/select.h>


#include "Client.hpp"
#include "Channel.hpp"
#include "CodeMap.hpp"

#define LIST_START "322"
#define LIST_END "323"
#define WHO_START "352"
#define WHO_END "315"

class Channel;
class Client;


class Bot
{
	private:
		int _sock;
		const char *_nick;
		size_t _current_channel;
		size_t _channel_known;
		std::vector<Channel> _channel;
		time_t _last_check;
		static const int	check_interval = 5;

	public:
		Bot();
		~Bot();
		void server_authentification();
		int socket_creation(int argc, char* argv[]);
		void communication_loop();
		void list_channels_handler(std::string &);
		void list_users_handler(std::string &packet);
		void message_reception(std::string &packet);
		void handle_global_data();
		void user_command();
		void operator_modification(std::string &packet);

};


#endif