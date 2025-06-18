#include "Bot.hpp"


/**
 * @brief Handle operator modification packets from MODE command
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
 * bite!bite@192.168.65.1 MODE #test +o Serverbot
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
	if (channel.empty() || grade.empty() || target.empty())
	{
		std::cerr << "Error parsing operator modification packet." << std::endl; //TODO: Debug message
		return;
	}
	try{
		_current_channel = find_channel_index(_channel, channel);
		if (_current_channel >= _channel.size() || _channel[_current_channel].getName() != channel)
		{
			std::cerr << "Channel " << channel << " not found in known channels. [operator]" << std::endl; // TODO: Debug message
			return;
		}
		if (grade == "+o")
		{
			_channel[_current_channel].setOp(true);
			std::cout << "Setting bots operator status to true for channel: " << channel << std::endl; // TODO: Debug message

		}
		else if (grade == "-o")
			_channel[_current_channel].setOp(false);
	}
	catch(...)
	{
		std::cerr << "Error finding channel index or parsing operator modification packet." << std::endl; //TODO: Debug message
		return;
	}
}
