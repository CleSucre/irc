#include "Bot.hpp"
/** 
	TODO: Create defines for the IRC codes
		 
*/

Bot::Bot() : _sock(-1), _nick("bot"), _current_channel(0), _channel_known(0), _last_check(std::time(NULL)) {}

Bot::~Bot()
{
	if (_sock != -1) {
		close(_sock);
	}
}

/**
 * @brief Parse a packet received from the IRC server
 * @param packet The packet to parse
 * @param end_code The end code to look for in the packet, split position
 * @return 0 on success, -1 on error
 * @note This function is currently not implemented.
 * It should parse the packet and return the appropriate value based on the end_code.
 * LIST end code is 323
 * User end code is 315
 * PRIVMSG end code is 401
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
 * 
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
	(void)_channel_known;
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
 * 
 * 	Server first send code 321, then 322 with server, then 323 to end the list
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
			return;
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
int copy_users(std::string src, std::vector<Client> &dest)
{
	if (src.empty())
		return (1);
	for(std::vector<Client>::iterator it = dest.begin(); it != dest.end(); ++it)
	{
		if (it->getUsername() == src)
			return (1);
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
 * 
 * Server first send code 352, then 315 to end the list
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
		copy_users(packet, _channel[_current_channel - 1].getClients());
	}
	catch (const std::runtime_error &e)
	{
		std::cerr << "Error finding channel index: " << e.what() << std::endl;
		return;
	}
}

/**
 * @brief Check for flood conditions
 * This function checks if the bot is flooding the server with messages.
 * If the bot sends too many messages in a short period, it will be disconnected.
 * TODO: modify recv by select
 * 	Server first send code 401, then 315 to end the list
 */
void Bot::message_reception()
{
	char buffer[512] = {0};
	for (int i = 0; i < 10; ++i)
	{
		usleep(500);
		int len = recv(_sock, buffer, sizeof(buffer)-1, 0);
		if (len <= 0) {
			std::cerr << "Connection closed or error occurred\n";
			return;
		}
		buffer[len] = '\0';
		std::string line(buffer);
		if (line.find("PRIVMSG :") == 0) { 
			std::cout << "Message received: " << line << std::endl; //TODO:Debug message
			// Need to parse message, check channel and compare last message time w current time
		}

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
		return;
	}
	buffer[len] = '\0';
	std::string packet(buffer);
	//TODO: put define instead of 322 or 323
	if (packet.find("322") != std::string::npos)
		list_channels_handler(packet);
	else if (packet.find("352") != std::string::npos || packet.find("315") != std::string::npos)
		list_users_handler(packet);
	// else if (packet.find("PRIVMSG") != std::string::npos)
	// 	message_reception();
	// else
	// 	std::cout << "Unhandled packet: " << packet << std::endl; // TODO: Debug message
}


void Bot::user_command()
{
	for (std::vector<Channel>::iterator it = _channel.begin(); it != _channel.end(); ++it)
	{
		std::string whoCmd = "WHO " + it->getName() + "\r\n";
		send(_sock, whoCmd.c_str(), whoCmd.size(), 0);
	}
}

/**
 * @brief Loop to communicate with the IRC server
 * This function handles sending and receiving messages, checking for new channel,
 *  clients, and processing incoming messages.
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
    while (true) {
		read_fds = master_fds;
		if (select(max_fd + 1, &read_fds, NULL, NULL, &timeout) < 0) {
			std::cerr << "Select error\n";
			return;
		}
		if (FD_ISSET(_sock, &read_fds))
			handle_global_data();
		time_t current_time = std::time(NULL);
		if (current_time - _last_check >= check_interval)
		{
			send(_sock, "LIST\r\n", 6, 0);
			user_command();
			// SEND LIST AND WHO COMMANDS
			_last_check = current_time;
		}
		//message_reception();
	}
    return ;
}

void Bot::server_authentification()
{
    std::string nickCmd = "NICK " + std::string(_nick) + "\r\n";
    std::string userCmd = "USER " + std::string(_nick) + " 0 * :Server bot\r\n";
    std::cout << "NICK command sent\n" << std::endl;
    send(_sock, nickCmd.c_str(), nickCmd.size(), 0);
	usleep(500);
    std::cout << "USER command sent\n" << std::endl;
    send(_sock, userCmd.c_str(), userCmd.size(), 0);
    usleep(500);
    std::cout << "Authentication complete, entering communication loop...\n" << std::endl;
    communication_loop();


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