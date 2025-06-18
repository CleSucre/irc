#include "Bot.hpp"

int Bot::check_flooding(Client *tmp, t_message &msg)
{
	if (tmp->getLastMessage() != 0 && tmp->getLastMessage() - std::time(NULL) > check_interval)
		{
			int warning_count = tmp->getWarningCount();
			tmp->setWarningCount(warning_count + 1);
			if (warning_count >= 3)
			{
				std::cout << "Flood detected from user: " << tmp->getNick() << ", disconnecting..." << std::endl; // TODO: Log message
				std::string kickCmd = "KICK " + msg.channel + " " + tmp->getNick() + " :Flood detected\r\n";
				send(_sock, kickCmd.c_str(), kickCmd.size(), 0);
				usleep(100);
				std::string privmsg = "PRIVMSG " + msg.username + " :You have been kicked for flooding\r\n";
				send(_sock, privmsg.c_str(), privmsg.size(), 0);
				usleep(50);
				return (1);
			}
			else
			{
				std::cout << "Warning sent to user: " << tmp->getNick() << ", warning count: " << warning_count + 1 << std::endl; // TODO: Log message
				std::string privmsg = "PRIVMSG " + msg.username + " :You have been warned for flooding\r\n";
				send(_sock, privmsg.c_str(), privmsg.size(), 0);
				usleep(50);
			}
		}
	return(0);
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

	try{
		_current_channel = find_channel_index(_channel, msg.channel);
		Channel *tmp_channel = getChannelbyId(_current_channel);
		if (tmp_channel == NULL || tmp_channel->getName().empty())
			return;
		if (tmp_channel->getClientsList().empty())
			return;
		if (tmp_channel->getOp() == false)
		{
			std::cerr << "Bot is not an operator in the channel " << msg.channel << ", cannot process message" << std::endl; // TODO: Log message
			return;
		}

		Client *tmp = tmp_channel->getClientbyNick(msg.username);
		if (tmp == NULL)
			return;
		if (check_flooding(tmp, msg) == 1)
			return;
		tmp->resetLastMessage();
	}
	catch (const std::runtime_error &e)
	{
		std::cerr << "Error: " << e.what() << std::endl; //TODO: Log message
		return;
	}
}

