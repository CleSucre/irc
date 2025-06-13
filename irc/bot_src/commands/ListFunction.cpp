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
 * @brief Get the channels from a packet
 * @param packet The packet received from the IRC server
 * @return A vector of channel names extracted from the packet
 * @note
 * This function parses the packet from List to extract channel names.
 * It looks for the LIST_START and CHANNEL_START markers to identify channel entries and which
 * position to split the packet.
 */
std::vector<std::string> get_packet_channels(std::string &packet)
{
	CodeMap code_map;
	std::string token;
	std::vector<std::string> channels;

	while(packet.size() )
	{
		size_t pos = packet.find(LIST_START);
		if (pos == std::string::npos || pos >= packet.size() || pos <= 0)
			break ;
		size_t next_pos = packet.find("\n", pos + 1);
		if (next_pos == std::string::npos)
			next_pos = packet.size();
		token = packet.substr(pos);
		packet.erase(0, next_pos);
		parse_packet(token, code_map.getIndex(CHANNEL_START));
		if (token.empty())
			break ;
		channels.push_back(token);
	}
	return channels;
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
	std::vector<std::string> channels;
	std::string token;

	channels = get_packet_channels(packet);
	if (channels.size() == 0)
	{
		std::cerr << "Error: No channels found in LIST command response." << std::endl; // TODO: Debug message
		return ;
	}
	// Join channels that are not already known
	for(std::vector<std::string>::iterator it = channels.begin(); it != channels.end(); ++it)
	{
		parse_packet(*it, code_map.getIndex(*it));
		if (it->empty())
			continue;
		if (!copy_channel(*it, _channel, _channel_known))
		{
			std::string jointCmd = "JOIN " + *it + "\r\n";
			if (send(_sock, jointCmd.c_str(), jointCmd.size(), 0) < 0)
			{
				std::cerr << "Error sending JOIN command for channel: " << *it << std::endl;
				return ;
			}
			std::cout << "Joining channel: " << *it << std::endl; // TODO: Debug message
			usleep(50);
		}
	}
}
