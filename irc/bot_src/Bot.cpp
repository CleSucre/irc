#include "Bot.hpp"
/** 
	TODO: Create defines for the IRC codes
		 
*/

Bot::Bot() : _sock(-1), _nick("bot"), _current_channel(0), _channel_known(0) {}

Bot::~Bot()
{
	if (_sock != -1) {
		close(_sock);
	}
}

/**
 * @brief Parse a packet received from the IRC server
 * @param packet The packet to parse
 * @param end_code The end code to look for in the packet
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
 * @brief Join channels on the IRC server
 * @param channels A vector of channel names to join
 * This function sends a JOIN command for each channel in the provided vector.
 * It will print a message for each channel it attempts to join.
 */
void Bot::join_channels(std::vector<std::string> &channels)
{
	for (std::vector<std::string>::iterator it = channels.begin(); it != channels.end(); it++)
	{
		std::string jointCmd = "JOIN " + *it + "\r\n";
		if (send(_sock, jointCmd.c_str(), jointCmd.size(), 0) < 0)
		{
			std::cerr << "Error sending JOIN command for channel: " << *it << std::endl;
			continue;
		}
		std::cout << "Joining channel: " << *it << std::endl; // TODO: Debug message
		usleep(500);
	}
}

/**
 * @brief Copy a list of channel names to a vector of Channel objects
 * @param src Source vector of channel names
 * @param dest Destination vector of Channel objects
 * @note This function does not handle duplicate channel names in the source vector.
 * This function checks if each channel name in the source vector exists in the destination vector.
 * If it does not exist, it creates a new Channel object and adds it to the destination vector.
 * The ID is set to the current size of the destination vector plus one.
 * It will only add unique channel names to the destination vector and suppress src names already in dest.
 */
void copy_list(std::vector<std::string> &src, std::vector<Channel> &dest)
{
	CodeMap code_map;
	for (std::vector<std::string>::iterator it = src.begin(); it != src.end(); )
	{
		if(it->empty())
		{
			++it;
			continue;
		}
		bool match = false;
		for(std::vector<Channel>::iterator jt = dest.begin(); jt != dest.end(); ++jt)
		{
			if (jt->getName() == *it)
			{
				match = true;
				it = src.erase(it);
				break ;
			}
		}
		if (match == false)
		{
			parse_packet(*it, code_map.getIndex(*it));
			dest.push_back(Channel(*it, dest.size() + 1));
			++it;
		}
	}
}

/**
 * @brief List channels on the IRC server
 * This function sends a LIST command to the IRC server and processes the response.
 * It updates the list of known channels and prints new channels found.
 * TODO: modify recv by select
 * 	Server first send code 321, then 322 with server, then 323 to end the list
 * 	We should check the code and parse the channel name and other information
 */
void Bot::list_channels()
{
	size_t channel_nb = 0;
	char buffer[512] = {0};
	std::vector<std::string> channels;
	
	std::cout << "Get server list\n"; //TODO:Debug message
	send(_sock, "LIST\r\n", 6, 0);

	// 323 is the response code for "End of /LIST command"
	// It could be change by END or other code, but 323 is the most common
	// TODO: put define here
	while (strstr(buffer, "323") == NULL)
	{
		int len = recv(_sock, buffer, sizeof(buffer)-1, 0);
		if (len <= 0)
		{
		    std::cerr << "Connection closed or error occurred\n";
		    break;
		}
		buffer[len] = '\0';
		channel_nb++;
		std::cout << "Received: " << buffer << std::endl; // TODO: Debug message
		std::cout << buffer;
		channels.push_back(std::string(buffer));
	}
	if (channel_nb > _channel_known)
	{
		std::cout << "New channels found: " << channel_nb - _channel_known << "\n";  //TODO:Debug message
		copy_list(channels, _channel);
		_channel_known = channel_nb;
		std::cout << "Total channels known update : " << _channel_known << "\n";  //TODO:Debug message
		join_channels(channels);

	}
	else
		std::cout << "No new channels found.\n";  // TODO: Debug message
}

/**
 * @brief Copy a list of usernames to a vector of Client objects
 * @param src Source vector of usernames
 * @param dest Destination vector of Client objects
 * @note This function does not handle duplicate usernames in the source vector.
 * This function checks if each username in the source vector exists in the destination vector.
 * If it does not exist, it creates a new Client object and adds it to the destination vector.
 * The ID is set to the current size of the destination vector plus one.
 * It will only add unique usernames to the destination vector.
 * TODO: Parse usernames correctly from the source vector
 */
void copy_users(std::vector<std::string> &src, std::vector<Client> &dest)
{
	CodeMap code_map;
	for (std::vector<std::string>::iterator it = src.begin(); it != src.end(); ++it)
	{
		if(it->empty())
			continue;
		bool match = false;
		for(std::vector<Client>::iterator jt = dest.begin(); jt != dest.end(); ++jt)
		{
			if (jt->getUsername() == *it)
			{
				match = true;
				break ;
			}
		}
		if (!match)
		{
			size_t id = dest.size() + 1;
			parse_packet(*it, code_map.getIndex(*it));
			dest.push_back(Client(*it, "unknown", id));
			std::cout << "New user added: " << *it << " with ID: " << id << "\n"; // TODO: Debug message
		}
	}
}

/**
 * @brief List users in each channel on the IRC server
 * This function sends a WHO command for each channel and processes the response.
 * It updates the list of known users in each channel and prints new users found.
 * TODO: modify recv by select
 * 	Server first send code 352, then 315 to end the list
 */
void Bot::list_users()
{
	size_t user_nb = 0;
	char buffer[512] = {0};
	std::vector<std::string> users;
	std::cout << "Get server users\n"; //TODO:Debug message
	for (std::vector<Channel>::iterator it = _channel.begin(); it != _channel.end(); ++it)
	{
		std::string channelName = it->getName();
		std::cout << "Channel: " << channelName << "\n"; // TODO: Debug message
		std::string whoCmd = "WHO " + channelName + "\r\n";
		send(_sock, whoCmd.c_str(), whoCmd.size(), 0);

		//315 is the response code for "End of /WHO command"
		// It could be change by END or other code, but 315 is the most common
		while (strstr(buffer, "315") == NULL)
		{
			int len = recv(_sock, buffer, sizeof(buffer)-1, 0);
			if (len <= 0)
			{
			    std::cerr << "Connection closed or error occurred\n";
			    break;
			}
			buffer[len] = '\0';
			user_nb++;
			std::cout << "Received: " << buffer << std::endl; // TODO: Debug message
			users.push_back(std::string(buffer));
		}
		if (user_nb > 0)
		{
			std::cout << "Users found: " << user_nb << "\n";  //TODO:Debug message
			copy_users(users, it->getClients());
		}
		else
			std::cout << "No users found.\n";  // TODO: Debug message
		usleep(200);
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
 * @brief Loop to communicate with the IRC server
 * This function handles sending and receiving messages, checking for new channel,
 *  clients, and processing incoming messages.
 */
void Bot::communication_loop()
{
    // Loop to send LIST, USER and check PRIVMSG
    while (true) {
		list_channels();
		list_users();
		//message_reception();


		//////////////////////////////
		// Check messages

		// char buffer[512];
        // std::string line(buffer);
        // if (line.find("PRIVMSG :") == 0) { //TODO:Debug message
        //     std::cout << "Message receive : " << line << std::endl; //TODO:Debug message
        //     // std::string token = line.substr(6);
        //     // std::string pong = "PONG :" + token + "\r\n";
        //     // send(sock, pong.c_str(), pong.size(), 0);
        //     // std::cout << ">> " << pong;
        // }
		// else
		// 	std::cout << "No message received\n"; //TODO:Debug message
		//////////////////////////////
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