#include "Bot.hpp"


// /**
//  * @brief Copy a username to a vector of Client objects
//  * @param src_name Source std::string containing the username
//  * @param dest Destination vector of Client objects
//  * @note
//  * This function checks if  username in the source exists in the destination vector.
//  * If it does not exist, it creates a new Client object and adds it to the destination vector.
//  * The ID is set to the current size of the destination vector plus one.
//  * It will only add unique usernames to the destination vector.
//  */
// int Bot::copy_users(std::string src_name, std::vector<Channel> &channel, size_t _current_channel)
// {
// 	std::vector<Client> &client_vec = channel[_current_channel].getClientsList();
// 	std::cout << "Searching a client named : [" << src_name << "]" << std::endl;
// 	if (src_name.empty())
// 	{
// 		if (src_name.empty())
// 			std::cerr << "Error: Empty username received." << std::endl; // TODO: Debug message
// 		return (1);
// 	}
// 	std::cout << "Copying user: " << src_name << " to channel: " << channel[_current_channel].getName() << std::endl; // TODO: Debug message
// 	/* This loop is to find if client already exit or not */
// 	for(std::vector<Client>::iterator it = client_vec.begin(); it != client_vec.end(); ++it)
// 	{
// 		std::cout << "Checking user: " << it->getUsername() << " with ID: " << it->getId() << std::endl; // TODO: Debug message
// 		if (it->getUsername() == src_name)
// 		{
// 			std::cout << "User found: " << src_name << " with ID: " << it->getId() << std::endl; // TODO: Debug message
// 			if (it->getWarningCount() >= 5)
// 			{
// 				std::string kickCmd = "KICK " + channel[_current_channel].getName() + " " + it->getNick() + " :Too many warnings\r\n";
// 				if (send(channel[_current_channel].getClientsList()[0].getId(), kickCmd.c_str(), kickCmd.size(), 0) < 0)
// 					return (1);
// 				std::string privmsg = "PRIVMSG " + it->getNick() + " :You have been banned for too many flooding\r\n";
// 				if (send(channel[_current_channel].getClientsList()[0].getId(), privmsg.c_str(), privmsg.size(), 0) < 0)
// 					return (1);
// 				std::cout << "Kicking user: " << it->getNick() << " from channel: " << channel[_current_channel].getName() << std::endl; // TODO: Debug message
// 			}
// 			std::cout << "User already exists: " << src_name << " with ID: " << it->getId() << std::endl; // TODO: Debug message
// 			return (1);
// 		}
// 	}
// 	client_vec.push_back(Client(src_name, src_name, client_vec.size() + 1));
// 	std::cout << "New user added: " << src_name << " with ID: " << client_vec.size() + 1 << "\n"; // TODO: Debug 
// 	// _channel[_current_channel].addClient(Client(src_name, src_name, client_vec.size() + 1));
// 	return (0);
// }

std::vector<std::pair<std::string, std::string> > get_packet_users(std::string &packet)
{
	CodeMap code_map;
	std::string token;
	std::string channel_token;
	std::vector<std::pair<std::string, std::string> > res;
	std::cout << "-----Parse Packet Users------ \n" << std::endl; // TODO: Debug message
	std::cout << "Received packet: " << packet << std::endl; // TODO: Debug message
	while (packet.size())
	{
		size_t pos = packet.find(WHO_START);
		if (pos == std::string::npos || pos >= packet.size() || pos <= 0)
			break;
		size_t next_pos = packet.find("\n", pos + 1);
		if (next_pos == std::string::npos)
			next_pos = packet.size();
		token = packet.substr(pos);
		std::cout << "Here is token before parse: " << token << std::endl; // TODO: Debug message
		channel_token = token;
		packet.erase(0, next_pos);
		parse_packet(channel_token, 4);
		parse_packet(token, 3);
		if (token.empty() || channel_token.empty())
			break;
		std::cout << "Here is token after parse: " << token << std::endl; // TODO: Debug message
		std::cout << "Here is channel token after parse: " << channel_token << std::endl; // TODO: Debug message
		res.push_back(std::make_pair(channel_token, token));
	}
	std::cout << "Parsed users before sort: " << std::endl; // TODO: Debug message
	for (std::vector<std::pair<std::string, std::string> >::iterator it = res.begin(); it != res.end(); ++it)
	{
		std::cout << "User: [" << it->first << "] in channel: [" << it->second << "]" << std::endl; // TODO: Debug message
	}
	std::sort(res.begin(), res.end());
	std::cout << "Parsed users: " << std::endl; // TODO: Debug message
	for (std::vector<std::pair<std::string, std::string> >::iterator it = res.begin(); it != res.end(); ++it)
	{
		std::cout << "User: [" << it->first << "] in channel: [" << it->second << "]" << std::endl; // TODO: Debug message
	}
	return res;
}

int copy_user(std::string username, std::string channel_name, std::vector<Channel> &channel_book)
{
	if (username.empty() || channel_name.empty())
	{
		std::cerr << "Error: Empty username or channel name received." << std::endl; // TODO: Debug message
		return (1);
	}
	// Create a vector of client from the channel book and search for username
	std::cerr << "Test segfault here" << std::endl; // TODO: Debug message
	size_t current_channel = find_channel_index(channel_book, channel_name);
	std::cerr << "Program pass test segfault here" << std::endl; // TODO: Debug message
	std::vector <Client> &client_book = channel_book[current_channel].getClientsList();
	// Search for the username in the client book (Loop to get all clients in channel)
	for (std::vector<Client>::iterator it = client_book.begin(); it != client_book.end(); ++it)
	{
		std::cout << "Checking user: [" << it->getUsername() << "] in channel: [" << channel_name << "]" << std::endl; // TODO: Debug message
		if (it->getUsername() == username)
		{
			std::cout << "User: [" << username << "] already exists in channel: [" << channel_name << "]" << std::endl; // TODO: Debug message
			if (it->getWarningCount() >= 5)
			{
				std::string kickCmd = "KICK " + channel_book[current_channel].getName() + " " + it->getNick() + " :Too many warnings\r\n";
				send(channel_book[current_channel].getClientsList()[0].getId(), kickCmd.c_str(), kickCmd.size(), 0);
				usleep(50);
				std::string privmsg = "PRIVMSG " + it->getNick() + " :You have been banned for too many flooding\r\n";
				send(channel_book[current_channel].getClientsList()[0].getId(), privmsg.c_str(), privmsg.size(), 0);
				std::cout << "Kicking user: " << it->getNick() << " from channel: " << channel_book[current_channel].getName() << std::endl; // TODO: Log message
				usleep(50);
			}
			return (1);
		}
	}
	client_book.push_back(Client(username, username, client_book.size() + 1));
	std::cout << "New user added: " << username << " to channel: " << channel_name << std::endl; // TODO: Debug message
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
 * @example
 * Example packet: ":irc.example.com 352 bot #channel nick user host server :realname\r\n"
 * 
 */
void Bot::list_users_handler(std::string &packet)
{
	CodeMap code_map;
	std::vector<std::pair<std::string, std::string> > users_list;
	
	std::cout << "-----HANDLE WHO RESPONSE ------ \n" << std::endl; // TODO: Debug message
	users_list = get_packet_users(packet);
	if (users_list.size() == 0)
	{
		std::cerr << "No users found in WHO command response." << std::endl; // TODO: Debug message
		return ;
	}

	for(std::vector<std::pair<std::string, std::string> >::iterator it = users_list.begin(); \
		it != users_list.end(); ++it)
	{
		if (!copy_user(it->first, it->second, _channel))
		{
			std::cout << "User: [" << it->first << "] added to channel: [" << it->second << "]" << std::endl; // TODO: Debug message
		}
		else
		{
			std::cout << "User: [" << it->first << "] already exists in channel: [" << it->second << "]" << std::endl; // TODO: Debug message
		}
	}
		std::cout << "--------END WHO HANDLING USERS-----------------\n" << std::endl; // TODO: Debug message

	// std::string channel_name;

	// channel_name = packet;
	// std::cout << "Received WHO response packet: " << packet << std::endl; // TODO: Debug message
	// parse_packet(packet, code_map.getIndex(packet));
	// std::cout << "Parsed WHO response packet: " << packet << std::endl; // TODO: Debug message
	// parse_packet(channel_name, 4);
	// std::cout << "Parsed WHO response channel_name : " << channel_name << std::endl; // TODO: Debug message
	// if (packet.empty() || channel_name.empty())
	// 	return;
	// try
	// {
	// 	_current_channel = find_channel_index(_channel, channel_name);
	// 	std::cout << "Current channel name: " << channel_name << std::endl; // TODO: Debug message
	// 	std::cout << "Current channel index: " << _current_channel << std::endl; // TODO: Debug message
	// 	Channel *tmp_channel = getChannelbyId(_current_channel);
	// 	if (tmp_channel == NULL || tmp_channel->getName().empty())
	// 	{
	// 		std::cerr << "Channel " << channel_name << " not found in known channels. [messages]" << std::endl; // TODO: Debug message
	// 		if (tmp_channel == NULL)
	// 			std::cerr << "Channel pointer is NULL." << std::endl; // TODO: Debug message
	// 		else
	// 			std::cerr << "Channel name is empty." << std::endl; // TODO: Debug message
	// 	}
	// 	std::cout << "Before copy_users" << std::endl; // TODO: Debug message
	// 	tmp_channel->print_client_in_channel(); // TODO: Debug message

	// 	std::cout << "\n\n----------COPY USERS-----------------" << std::endl; // TODO: Debug message
	// 	copy_users(packet, _channel, _current_channel);

	// 	std::cout << "After copy_users" << std::endl; // TODO: Debug message
	// 	tmp_channel->print_client_in_channel(); // TODO: Debug message

	// 	std::cout << "--------END COPY USERS-----------------\n" << std::endl; // TODO: Debug message

	// }
	// catch (const std::runtime_error &e)
	// {
	// 	std::cerr << "Error finding channel index in list_user: " << e.what() << std::endl;
	// 	return;
	// }
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
	std::cout << "Sending WHO command to all channels where the bot is an operator..." << std::endl; // TODO: Debug message
	for (std::vector<Channel>::iterator it = _channel.begin(); it != _channel.end(); ++it)
	{
		if (it->getOp() == false)
		{
			std::cout << "Bot is not an operator in channel: " << it->getName() << ", skipping WHO command." << std::endl; // TODO: Debug message
			continue;
		}
		std::cout << "Sending WHO command for channel: " << it->getName() << std::endl; // TODO: Debug message

		std::string whoCmd = "WHO " + it->getName() + "\r\n";
		send(_sock, whoCmd.c_str(), whoCmd.size(), 0);
		usleep(50);
		// TODO: Maybe add a delay here to avoid flooding the server with WHO commands
	}
}
