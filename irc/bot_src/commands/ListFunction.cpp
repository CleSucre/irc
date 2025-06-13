#include "Bot.hpp"

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
	std::cout << "Copying channel: " << src << std::endl; // TODO: Debug message
	for(std::vector<Channel>::iterator jt = dest.begin(); jt != dest.end(); ++jt)
	{
		if (jt->getName() == src)
			return (1);
	}
	dest.push_back(Channel(src, dest.size()));
	std::cout << "New channel added: " << src << " with ID: " << dest.size() << "\n"; // TODO: Debug message
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
	size_t pos = packet.find(LIST_START);
	for (size_t i = 0; i < pos; ++i)
	{
		if (packet[i] == '\r' || packet[i] == '\n')
		{
			packet.erase(0, i + 1);
			pos -= i + 1;
		}
	}
	// TODO: Create a loop bc bot get every servers in a single packet
	if (packet.empty())
	{
		std::cerr << "Error: Empty packet received for LIST command." << std::endl; // TODO: Debug message
		return ;
	}
	std::cout << "Received LIST command response: " << packet << std::endl; // TODO: Debug message
	parse_packet(packet, code_map.getIndex(packet));
	std::cout << "Parsed packet: " << packet << std::endl; // TODO: Debug message
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