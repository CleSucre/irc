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
#include <algorithm>



#include "Client.hpp"
#include "Channel.hpp"
#include "CodeMap.hpp"

#include <signal.h>

#define LIST_START "322"
#define LIST_END "323"
#define WHO_START "352"
#define WHO_END "315"
#define CHANNEL_START "B322"


class Channel;
class Client;


struct ping
{
	time_t _last_ping;
	bool _waiting_pong;
	std::string tokens;
	static const int _ping_delay = 10;
	static const int _pong_waiting_delay = 5;
};

/**
 * @brief Structure to hold message information
 * This structure holds the username, channel, and message content.
 * It is used to store the parsed information from a packet message.
 * @note
 * The structure is used in the message_reception function to process incoming messages.
 * It is designed to be simple and efficient for storing message data.
 * std::string message is used to store the message content, it is here if we want to add more bot rules later.
 */
struct t_message {
	std::string username;
	std::string channel;
	std::string message;
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
		static const int	flood_interval = 6;
		static const int	check_who_interval = 3;
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
		void interval_verification(time_t &, time_t &, time_t current_time);

		static void handleSignal(int sig);
		// Stock in channel.hpp for now
		Channel *getChannelbyName(const std::string &name);
		Channel *getChannelbyId(size_t id);
		int check_flooding(Client *tmp, t_message &msg);
};

int	parse_packet(std::string &packet, int split_position);
t_message split_packet_message(const std::string &packet);

#endif