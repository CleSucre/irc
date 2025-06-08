#include "Bot.hpp"

int Bot::_end_signal = 0;


Bot::Bot() : _sock(-1), _nick("bot"), _current_channel(0), _channel_known(0), _last_check(std::time(NULL)) {
	_ping_status._last_ping = std::time(NULL);
	_ping_status._waiting_pong = false;
}

Bot::~Bot()
{
	if (_sock != -1)
		close(_sock);
}

/**
 * @brief Parse a packet received from the IRC server
 * @param packet The packet to parse
 * @param end_code The end code to look for in the packet, split position
 * @return 0 on success, -1 on error
 * @note This function is currently not implemented.
 * It should parse the packet and return the appropriate value based on the end_code.
 * @example:
 * WHO command response:
 * :irc.example.com 352 julien #canal juliette example.com irc.example.com juliette H@ :0 Juliette Dupont
 * :serveur 352 <demandeur> <canal> <user> <host> <serveur> <nick> <status> :<hopcount> <realname>
 * Important element : 5th element
 * @example:
 * LIST command response:
 * :irc.example.com 322 julien #canal 10 :This is a test channel
 * :serveur 322 <demandeur> <canal> <nb_utilisateurs> :<topic>
 * Important element : 4th element
 */
int	parse_packet(std::string &packet, int split_position)
{
	if (split_position < 1) {
		std::cerr << "Error: Invalid split position." << std::endl;
		return (-1);
	}
	std::vector<std::string> tokens;
	tokens = split(packet, ' ');
	if (tokens.size() < (size_t) split_position) {
		std::cerr << "Error: Packet too small." << std::endl;
		return (-1);
	}
	packet = tokens[split_position - 1];
	if (packet.empty()) {
		std::cerr << "Error: Packet is empty." << std::endl;
		return (-1);
	}
	return (0);
}

/**
 * @brief Copy a channel name to a vector of Channel objects
 * @param src Source vector of channel names
 * @param dest Destination vector of Channel objects
 * This function checks if channel name in the source vector exists in the destination vector.
 * If it does not exist, it creates a new Channel object and adds it to the destination vector.
 * The ID is set to the current size of the destination vector plus one.
 */
int copy_channel(std::string &src, std::vector<Channel> &dest, int _channel_known)
{
	if (src.empty())
		return (1);
	for(std::vector<Channel>::iterator jt = dest.begin(); jt != dest.end(); ++jt)
	{
		if (jt->getName() == src)
			return (1);
	}
	dest.push_back(Channel(src, dest.size() + 1));
	_channel_known++;
	return (0);
}

/**
 * @brief Handle the LIST command response from the IRC server
 * @param packet The packet received from the IRC server
 * @note This function is called when the bot receives a LIST command response.
 * It parses the packet to extract the channel name and checks if it is already known.
 * If the channel is not known, it update the list of known channels 
 * then sends a JOIN command to the IRC server to join the channel.
 */
void Bot::list_channels_handler(std::string &packet)
{
	CodeMap code_map;

	parse_packet(packet, code_map.getIndex(packet));
	if (!copy_channel(packet, _channel, _channel_known))
	{
		std::string jointCmd = "JOIN " + packet + "\r\n";
		if (send(_sock, jointCmd.c_str(), jointCmd.size(), 0) < 0)
		{
			std::cerr << "Error sending JOIN command for channel: " << packet << std::endl;
			return ;
		}
		std::cout << "Joining channel: " << packet << std::endl; // TODO: Debug message
		usleep(50);
	}
}

/**
 * @brief Copy a username to a vector of Client objects
 * @param src Source std::string containing the username
 * @param dest Destination vector of Client objects
 * @note
 * This function checks if  username in the source exists in the destination vector.
 * If it does not exist, it creates a new Client object and adds it to the destination vector.
 * The ID is set to the current size of the destination vector plus one.
 * It will only add unique usernames to the destination vector.
 */
int copy_users(std::string src, std::vector<Channel> &channel, size_t _current_channel)
{
	std::vector<Client> &dest = channel[_current_channel - 1].getClientsList();
	if (src.empty())
		return (1);
	for(std::vector<Client>::iterator it = dest.begin(); it != dest.end(); ++it)
	{
		if (it->getUsername() == src)
		{
			if (it->getWarningCount() >= 5)
			{
				std::string kickCmd = "KICK " + channel[_current_channel - 1].getName() + " " + it->getNick() + " :Too many warnings\r\n";
				if (send(channel[_current_channel - 1].getClientsList()[0].getId(), kickCmd.c_str(), kickCmd.size(), 0) < 0)
					return (1);
				std::string privmsg = "PRIVMSG " + it->getNick() + " :You have been banned for too many flooding\r\n";
				if (send(channel[_current_channel - 1].getClientsList()[0].getId(), privmsg.c_str(), privmsg.size(), 0) < 0)
					return (1);
				std::cout << "Kicking user: " << it->getNick() << " from channel: " << channel[_current_channel - 1].getName() << std::endl; // TODO: Debug message
			}
			return (1);
		}
	}
	dest.push_back(Client(src, "unknown", dest.size() + 1));
	std::cout << "New user added: " << src << " with ID: " << dest.size() + 1 << "\n"; // TODO: Debug 
	return (0);
}

/**
 * @brief Handle the WHO command response from the IRC server
 * @param packet The packet received from the IRC server
 * @note
 * This function is called when the bot receives a WHO command response.
 * It parses the packet to extract the channel name and updates the list of known users in that channel.
 * It uses the CodeMap to determine the index of the packet and processes the response accordingly.
 * It updates the list of known users in each channel
 */
void Bot::list_users_handler(std::string &packet)
{
	CodeMap code_map;
	std::string tmp;

	tmp = packet;
	parse_packet(packet, code_map.getIndex(packet));
	parse_packet(tmp, 4);
	if (packet.empty() || tmp.empty())
		return;
	try
	{
		_current_channel = find_channel_index(_channel, tmp);
		copy_users(packet, _channel, _current_channel);
	}
	catch (const std::runtime_error &e)
	{
		std::cerr << "Error finding channel index: " << e.what() << std::endl;
		return;
	}
}


/**
 * @brief Split a packet message into tokens
 * @param packet The packet message to split
 * @return A t_message structure containing the username, channel, and message content
 * This function extracts the username, channel, and message content from the packet message.
 * It uses string manipulation to find the relevant parts of the packet and store them in a t_message structure.
 * @note
 * The packet message is expected to be in the format:
 * :username!user@host PRIVMSG #channel :message
 * The function assumes that the packet is well-formed and does not perform extensive error checking.
 */
t_message split_packet_message(const std::string &packet)
{
	t_message msg;
	msg.username = packet.substr(1, packet.find('!') - 1);
	size_t start = packet.find("PRIVMSG");
	start += 8;
	size_t end = packet.find(' ', start);
	msg.channel = packet.substr(start, end - start);
	start = packet.find(':',end) + 1;
	msg.message = packet.substr(start);
	return (msg);
}


/**
 * @brief Handle incoming messages from the IRC server
 * @param packet The packet message received from the IRC server
 * @note
 * This function processes incoming messages from the IRC server.
 * It checks if the message is from a known user and channel, and handles flood detection.
 * It updates the last message time for the user and checks for flood conditions.
 * If a flood is detected, it kicks the user from the channel and sends a private message.
 * @example:
 * Example of a packet received:
 * :username!user@host PRIVMSG #channel :message
 */
void Bot::message_reception(std::string &packet)
{
	t_message msg = split_packet_message(packet);
	std::cout << "Message received from " << msg.username << " in channel " << msg.channel << ": " << msg.message << std::endl; // TODO: Debug message
	try{
		_current_channel = find_channel_index(_channel, msg.channel);
		if (_channel[_current_channel - 1].getOp() == false)
			throw std::runtime_error("Bot is not an operator in the channel" + msg.channel + ", cannot process message");
		Client &tmp = _channel[_current_channel - 1].getClientbyNick(msg.username);
		if (tmp.getLastMessage() - std::time(NULL) > check_interval)
		{
			int warning_count = tmp.getWarningCount();
			tmp.setWarningCount(warning_count + 1);
			if (warning_count >= 3)
			{
				std::cout << "Flood detected from user: " << tmp.getNick() << ", disconnecting..." << std::endl; // TODO: Debug message
				std::string kickCmd = "KICK " + msg.channel + " " + tmp.getNick() + " :Flood detected\r\n";
				send(_sock, kickCmd.c_str(), kickCmd.size(), 0);
				usleep(50);
				std::string privmsg = "PRIVMSG " + msg.username + " :You have been kicked for flooding\r\n";
				send(_sock, privmsg.c_str(), privmsg.size(), 0);
				return;
			}
			else
			{
				std::cout << "Warning sent to user: " << tmp.getNick() << ", warning count: " << warning_count + 1 << std::endl; // TODO: Debug message
				std::string privmsg = "PRIVMSG " + msg.username + " :You have been warned for flooding\r\n";
				send(_sock, privmsg.c_str(), privmsg.size(), 0);
				usleep(50);
			}
		}
		tmp.resetLastMessage();
	}
	catch (const std::runtime_error &e)
	{
		std::cerr << "Error finding channel index: " << e.what() << std::endl; //TODO: Debug message
		return;
	}
}
/**
 * @brief Handle operator modification packets
 * @param packet The packet received from the IRC server
 * @note
 * This function processes operator modification packets, which are used to add or remove operators from a channel.
 * It checks if the packet is well-formed and contains the necessary information.
 * It updates the channel's operator status based on the packet content.
 * @note
 * Important elements:
 * - 3rd element: channel name
 * - 4th element: operator grade (+o or -o)
 * - 5th element: target username
 * @example:
 * Example of a packet received:
 * :irc.example.com MODE #channel +o username
 * :server MODE <channel> <grade> <target>
 */
void Bot::operator_modification(std::string &packet)
{
	if (packet.empty())
		return ;
	std::string grade = packet;
	std::string target = packet;
	std::string channel = packet;

	parse_packet(channel, 3);
	parse_packet(grade, 4);
	parse_packet(target, 5);
	if (channel.empty() || grade.empty() || target.empty() || target != _nick)
	{
		std::cerr << "Error parsing operator modification packet." << std::endl; //TODO: Debug message
		return;
	}
	try{
		_current_channel = find_channel_index(_channel, channel);
		if (grade == "+o")
			_channel[_current_channel].setOp(true);
		else if (grade == "-o")
			_channel[_current_channel].setOp(false);
	}
	catch(...)
	{
		std::cerr << "Error finding channel index or parsing operator modification packet." << std::endl; //TODO: Debug message
		return;
	}
}

/**
 * @brief Handle global data received from the IRC server
 * This function receives data from the IRC server and processes it.
 * It checks the type of packet received and calls the appropriate handler function.
 * It handles LIST and WHO commands responses, and can be extended to handle other types of packets.
 * @note
 * This function is called in the communication loop to handle incoming data from the server.
 * It uses the recv function to read data from the socket and processes it based on the packet type.
 * It currently handles LIST (322) and WHO (352, 315) commands responses.
 * It can be extended to handle other types of packets as needed.
 * @example:
 * Example of a packet received:
 * :irc.example.com 322 foo_user #canal 10 :This is a test channel
 */
void Bot::handle_global_data()
{
	char buffer[512] = {0};
	int len = recv(_sock, buffer, sizeof(buffer)-1, 0);
	if (len <= 0) {
		std::cerr << "Connection closed or error occurred\n";
		_end_signal = 1;
		return;
	}
	buffer[len] = '\0';
	std::string packet(buffer);
	if (packet.find(LIST_START) != std::string::npos)
		list_channels_handler(packet);
	else if (packet.find(WHO_START) != std::string::npos || packet.find(WHO_END) != std::string::npos)
		list_users_handler(packet);
	else if (packet.find("PRIVMSG") != std::string::npos)
		message_reception(packet);
	else if (packet.find("MODE") != std::string::npos)
		operator_modification(packet);
	else if (_ping_status._waiting_pong && packet.find("PONG :" + _ping_status.tokens) != std::string::npos)
		_ping_status._waiting_pong = false;
}

/**
 * @brief Send WHO command to all channels where the bot is an operator
 * This function sends a WHO command to each channel where the bot is an operator.
 * It iterates through the list of channels and sends the WHO command for each channel.
 * @note
 * The WHO command is used to get a list of users in a channel.
 * It is sent to the IRC server to retrieve information about users in each channel.
 */
void Bot::user_command()
{
	for (std::vector<Channel>::iterator it = _channel.begin(); it != _channel.end(); ++it)
	{
		if (it->getOp() == false)
			continue;
		std::string whoCmd = "WHO " + it->getName() + "\r\n";
		send(_sock, whoCmd.c_str(), whoCmd.size(), 0);
	}
}

/**
 * @brief Loop to communicate with the IRC server
 * This function handles sending and receiving messages, checking for new channel,
 *  clients, and processing incoming messages.
 * It uses the select function to wait for incoming data on the socket.
 * @note
 * The communication loop is the main part of the bot that keeps it running and interacting with the IRC server.
 * It is responsible for maintaining the connection, processing incoming messages, and sending commands.
 * It uses a timeout to check for new channels and users every 5 seconds.
 * It can be extended to handle more complex interactions with the IRC server, such as responding to specific commands or messages.
 */
void Bot::communication_loop()
{
	fd_set master_fds, read_fds;
	FD_ZERO(&master_fds);
	FD_SET(_sock, &master_fds);

	struct timeval timeout;
	timeout.tv_sec = 1;
	timeout.tv_usec = 0;
	int max_fd = _sock;
    while (_end_signal == 0) {
		time_t current_time = std::time(NULL);

		if (_ping_status._waiting_pong == false && current_time - _ping_status._last_ping >= _ping_status._ping_delay)
		{
			std::cout << "Sending PING command to server..." << std::endl; // TODO: Debug message
			_ping_status._last_ping = current_time;
			_ping_status.tokens = current_time;
			std::string pingCmd = "PING :" + _ping_status.tokens + "\r\n";
			send(_sock, pingCmd.c_str(), pingCmd.size(), 0);
			_ping_status._waiting_pong = true;
		}
		if (current_time - _last_check >= check_interval)
		{
			send(_sock, "LIST\r\n", 6, 0);
			user_command();
			_last_check = current_time;
		}
		read_fds = master_fds;
		if (select(max_fd + 1, &read_fds, NULL, NULL, &timeout) < 0) {
			std::cerr << "Select error\n";
			return;
		}
		if (FD_ISSET(_sock, &read_fds))
			handle_global_data();
		if (_ping_status._waiting_pong && current_time - _ping_status._last_ping >= _ping_status._pong_waiting_delay)
		{
			std::cerr << "No PONG received, server may be down or not responding." << std::endl;
			_end_signal = 1;
		}
	}
    return ;
}

void Bot::server_authentification()
{
    std::string nickCmd = "NICK " + std::string(_nick) + "\r\n";
    std::string userCmd = "USER " + std::string(_nick) + " 0 * :Server bot\r\n";
    std::cout << "NICK command sent" << std::endl;
    send(_sock, nickCmd.c_str(), nickCmd.size(), 0);
	usleep(100);
    std::cout << "USER command sent" << std::endl;
    send(_sock, userCmd.c_str(), userCmd.size(), 0);
    usleep(100);
    std::cout << "Authentication complete, entering communication loop..." << std::endl;
    communication_loop();
}


void Bot::handleSignal(int sig)
{
    if (sig == SIGINT || sig == SIGQUIT)
        Bot::_end_signal = 1;
}


/**
 * @brief Create a socket and connect bot to the IRC server
 * @param argc Number of command line arguments
 * @param argv Array of command line arguments
 * @return 0 on success, 1 on error
 */
int Bot::socket_creation(int argc, char* argv[])
{
    const char* server = (argc > 1 ? argv[1] : "127.0.0.1");
    int port          = (argc > 2 ? std::atoi(argv[2]) : 6667);
    _nick   = (argc > 3 ? argv[3] : "bot");

	signal(SIGINT, Bot::handleSignal);
	signal(SIGQUIT, Bot::handleSignal); 

    _sock = socket(AF_INET, SOCK_STREAM, 0);
    if (_sock < 0) {
        std::cerr << "Socket Error\n";
        return 1;
    }
    sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    if (inet_pton(AF_INET, server, &addr.sin_addr) <= 0) {
        std::cerr << "Invalid adress\n";
        return 1;
    }
    if (connect(_sock, (sockaddr*)&addr, sizeof(addr)) < 0) {
        std::cerr << "Impossible to connect to " << server << ":" << port << "\n";
        return 1;
    }
    std::cout << "Connected to " << server << ":" << port << "\n";
    server_authentification();
    close(_sock);
    return (0);
}