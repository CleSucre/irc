#include "Bot.hpp"

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
		std::cout << "Current channel index: " << _current_channel << std::endl; // TODO: Debug message
		print_all_channels(); // TODO: Debug message

		Channel *tmp_channel = getChannelbyId(_current_channel);
		if (tmp_channel == NULL || tmp_channel->getName().empty())
		{
			std::cerr << "Channel " << msg.channel << " not found in known channels. [messages]" << std::endl; // TODO: Debug message
			if (tmp_channel == NULL)
				std::cerr << "Channel pointer is NULL." << std::endl; // TODO: Debug message
			else
				std::cerr << "Channel name is empty." << std::endl; // TODO: Debug message
			return;
		}
		if (tmp_channel->getClientsList().empty())
		{
			std::cerr << "No clients in channel " << msg.channel << std::endl; // TODO: Debug message
			return;
		}
		if (tmp_channel->getOp() == false)
		{
			std::cerr << "Bot is not an operator in the channel " << msg.channel << ", cannot process message" << std::endl; // TODO: Debug message
			return;
		}
		std::cout << "---------------------------" << std::endl; // TODO: Debug message
		std::cout << "Here is tmp_channel name: " << tmp_channel->getName() << std::endl; // TODO: Debug message
		std::cout << "Here is tmp_channel ID: " << tmp_channel->getId() << std::endl; // TODO: Debug message
		std::cout << "Here is msg.username: " << msg.username << std::endl; // TODO: Debug message
		std::cout << "---------------------------" << std::endl; // TODO: Debug message

		Client *tmp = tmp_channel->getClientbyNick(msg.username);
		for (std::vector<Client>::iterator it = tmp_channel->getClientsList().begin(); it != tmp_channel->getClientsList().end(); ++it)
		{
			std::cout << "Client in channel: " << it->getNick() << ", Username: " << it->getUsername() 
					  << ", ID: " << it->getId() << std::endl; // TODO: Debug message
		}
		if (tmp == NULL)
		{
			std::cerr << "Error in getclientbyNick: " << msg.username << " not found in channel " << msg.channel << std::endl; // TODO: Debug message
			std::cerr << "User " << msg.username << " not found in channel " << msg.channel << std::endl; // TODO: Debug message
			return;
		}
		if (tmp->getLastMessage() != 0 && tmp->getLastMessage() - std::time(NULL) > check_interval)
		{
			int warning_count = tmp->getWarningCount();
			tmp->setWarningCount(warning_count + 1);
			if (warning_count >= 3)
			{
				std::cout << "Flood detected from user: " << tmp->getNick() << ", disconnecting..." << std::endl; // TODO: Debug message
				std::string kickCmd = "KICK " + msg.channel + " " + tmp->getNick() + " :Flood detected\r\n";
				send(_sock, kickCmd.c_str(), kickCmd.size(), 0);
				usleep(50);
				std::string privmsg = "PRIVMSG " + msg.username + " :You have been kicked for flooding\r\n";
				send(_sock, privmsg.c_str(), privmsg.size(), 0);
				return;
			}
			else
			{
				std::cout << "Warning sent to user: " << tmp->getNick() << ", warning count: " << warning_count + 1 << std::endl; // TODO: Debug message
				std::string privmsg = "PRIVMSG " + msg.username + " :You have been warned for flooding\r\n";
				send(_sock, privmsg.c_str(), privmsg.size(), 0);
				usleep(50);
			}
		}
		tmp->resetLastMessage();
	}
	catch (const std::runtime_error &e)
	{
		std::cerr << "Error finding channel index in message_reception: " << e.what() << std::endl; //TODO: Debug message
		return;
	}
}

