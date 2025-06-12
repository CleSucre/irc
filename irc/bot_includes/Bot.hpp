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

#include <signal.h>

#define LIST_START "322"
#define LIST_END "323"
#define WHO_START "352"
#define WHO_END "315"

class Channel;
class Client;


struct ping
{
	time_t _last_ping;
	bool _waiting_pong;
	std::string tokens;
	/// TODO: Minimize the PING delay, PONG isn't implemented yet
	static const int _ping_delay = 10000;
	static const int _pong_waiting_delay = 5000;
};

class Bot
{
	private:
		int _sock;
		const char *_nick;
		const char *_password;
		const char *_username;
		size_t _current_channel;
		size_t _channel_known;
		std::vector<Channel> _channel;
		time_t _last_check;
		static const int	check_interval = 5;
		static int	_end_signal;
		ping _ping_status;

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

		static void handleSignal(int sig);
		// Stock in channel.hpp for now
		Channel *getChannelbyName(const std::string &name);
		Channel *getChannelbyId(size_t id);
		int copy_users(std::string src_name, std::vector<Channel> &channel, size_t _current_channel);




		// DEBUG BY CHATGPT
		void print_all_channels();
		void list_channels_known();

};

int copy_users(std::string src_name, std::vector<Channel> &channel, size_t _current_channel);


#endif