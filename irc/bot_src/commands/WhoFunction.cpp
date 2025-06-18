#include "Bot.hpp"

/**
 * @brief Parse the WHO command response packet to extract users and their channels
 * @param packet The packet received from the IRC server
 * @return A vector of pairs containing the username and the channel name
 * @note
 * This function parses the WHO command response packet to extract the users and their channels.
 */
std::vector<std::pair<std::string, std::string> > get_packet_users(std::string &packet)
{
	CodeMap code_map;
	std::string token;
	std::string channel_token;
	std::vector<std::pair<std::string, std::string> > res;
	while (packet.size())
	{
		size_t pos = packet.find(WHO_START);
		if (pos == std::string::npos || pos >= packet.size() || pos <= 0)
			break;
		size_t next_pos = packet.find("\n", pos + 1);
		if (next_pos == std::string::npos)
			next_pos = packet.size();
		token = packet.substr(pos);
		channel_token = token;
		packet.erase(0, next_pos);
		parse_packet(channel_token, 4);
		parse_packet(token, 3);
		if (token.empty() || channel_token.empty())
			break;
		res.push_back(std::make_pair(channel_token, token));
	}
	std::sort(res.begin(), res.end());
	return res;
}

/**
 * @brief Copy user to the channel book
 * @param username The username of the user to copy
 * @param channel_name The name of the channel to copy the user to
 * @param channel_book The vector of channels to copy the user to
 * @return 0 if the user was copied successfully, 1 if the user already exists in the channel
 * @note
 * This function checks if the user already exists in the channel book.
 * If the user exists and has more than 5 warnings, it kicks the user from the channel.
 * If the user does not exist, it adds the user to the channel book.
 */
int copy_user(std::string username, std::string channel_name, std::vector<Channel> &channel_book)
{
	if (username.empty() || channel_name.empty())
	{
		std::cerr << "Error: Empty username or channel name received." << std::endl; //TODO log message
		return (1);
	}
	size_t current_channel = find_channel_index(channel_book, channel_name);
	std::vector <Client> &client_book = channel_book[current_channel].getClientsList();
	for (std::vector<Client>::iterator it = client_book.begin(); it != client_book.end(); ++it)
	{
		if (it->getUsername() == username)
		{
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
	std::cout << "New user added: " << username << " to channel: " << channel_name << std::endl; // TODO: Log message
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
	
	users_list = get_packet_users(packet);
	if (users_list.size() == 0)
		return ;

	for(std::vector<std::pair<std::string, std::string> >::iterator it = users_list.begin(); \
		it != users_list.end(); ++it)
	{
		if (!copy_user(it->first, it->second, _channel))
			std::cout << "User: [" << it->first << "] added to channel: [" << it->second << "]" << std::endl; // TODO: log message
		else
			std::cout << "User: [" << it->first << "] already exists in channel: [" << it->second << "]" << std::endl; // TODO: log message -- not sure if will be keepd
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
	for (std::vector<Channel>::iterator it = _channel.begin(); it != _channel.end(); ++it)
	{
		if (it->getOp() == false)
			continue;
		std::string whoCmd = "WHO " + it->getName() + "\r\n";
		send(_sock, whoCmd.c_str(), whoCmd.size(), 0);
		usleep(50);
	}
}
