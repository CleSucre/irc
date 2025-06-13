#include "Bot.hpp"


/**
 * @brief Copy a username to a vector of Client objects
 * @param src_name Source std::string containing the username
 * @param dest Destination vector of Client objects
 * @note
 * This function checks if  username in the source exists in the destination vector.
 * If it does not exist, it creates a new Client object and adds it to the destination vector.
 * The ID is set to the current size of the destination vector plus one.
 * It will only add unique usernames to the destination vector.
 */
int Bot::copy_users(std::string src_name, std::vector<Channel> &channel, size_t _current_channel)
{
	std::vector<Client> &client_vec = channel[_current_channel].getClientsList();
	std::cout << "Searching a client named : [" << src_name << "]" << std::endl;
	if (src_name.empty())
	{
		if (src_name.empty())
			std::cerr << "Error: Empty username received." << std::endl; // TODO: Debug message
		return (1);
	}
	std::cout << "Copying user: " << src_name << " to channel: " << channel[_current_channel].getName() << std::endl; // TODO: Debug message
	for(std::vector<Client>::iterator it = client_vec.begin(); it != client_vec.end(); ++it)
	{
		std::cout << "Checking user: " << it->getUsername() << " with ID: " << it->getId() << std::endl; // TODO: Debug message
		if (it->getUsername() == src_name)
		{
			std::cout << "User found: " << src_name << " with ID: " << it->getId() << std::endl; // TODO: Debug message
			if (it->getWarningCount() >= 5)
			{
				std::string kickCmd = "KICK " + channel[_current_channel].getName() + " " + it->getNick() + " :Too many warnings\r\n";
				if (send(channel[_current_channel].getClientsList()[0].getId(), kickCmd.c_str(), kickCmd.size(), 0) < 0)
					return (1);
				std::string privmsg = "PRIVMSG " + it->getNick() + " :You have been banned for too many flooding\r\n";
				if (send(channel[_current_channel].getClientsList()[0].getId(), privmsg.c_str(), privmsg.size(), 0) < 0)
					return (1);
				std::cout << "Kicking user: " << it->getNick() << " from channel: " << channel[_current_channel].getName() << std::endl; // TODO: Debug message
			}
			std::cout << "User already exists: " << src_name << " with ID: " << it->getId() << std::endl; // TODO: Debug message
			return (1);
		}
	}
	client_vec.push_back(Client(src_name, src_name, client_vec.size() + 1));
	// _channel[_current_channel].addClient(Client(src_name, src_name, client_vec.size() + 1));
	std::cout << "New user added: " << src_name << " with ID: " << client_vec.size() + 1 << "\n"; // TODO: Debug 
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
	std::cout << "Received WHO command response: " << packet << std::endl; // TODO: Debug message
	try
	{
		_current_channel = find_channel_index(_channel, tmp);
		std::cout << "Current channel name: " << tmp << std::endl; // TODO: Debug message
		std::cout << "Current channel index: " << _current_channel << std::endl; // TODO: Debug message
		Channel *tmp_channel = getChannelbyId(_current_channel);
		if (tmp_channel == NULL || tmp_channel->getName().empty())
		{
			std::cerr << "Channel " << tmp << " not found in known channels. [messages]" << std::endl; // TODO: Debug message
			if (tmp_channel == NULL)
				std::cerr << "Channel pointer is NULL." << std::endl; // TODO: Debug message
			else
				std::cerr << "Channel name is empty." << std::endl; // TODO: Debug message
		}
		std::cout << "Before copy_users" << std::endl; // TODO: Debug message
		list_channels_known();

		copy_users(packet, _channel, _current_channel);
		
		std::cout << "End copy" << std::endl; // TODO: Debug message
	}
	catch (const std::runtime_error &e)
	{
		std::cerr << "Error finding channel index in list_user: " << e.what() << std::endl;
		return;
	}
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
