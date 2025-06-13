#include "Bot.hpp"


/**
 * @brief Parse a packet received from the IRC server
 * @param packet The packet to parse
 * @param end_code The end code to look for in the packet, split position
 * @return 0 on success, -1 on error
 * @note This function is currently not implemented.
 * It should parse the packet and return the appropriate value based on the end_code.
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
 * @brief Split a packet message into tokens
 * @param packet The packet message to split
 * @return A t_message structure containing the username, channel, and message content
 * This function extracts the username, channel, and message content from the packet message.
 * It uses string manipulation to find the relevant parts of the packet and store them in a t_message structure.
 * @note
 * The packet message is expected to be in the format:
 * :username!user@host PRIVMSG #channel :message
 * The function assumes that the packet is well-formed and does not perform extensive error checking.
 */
t_message split_packet_message(const std::string &packet)
{
	t_message msg;
	msg.username = packet.substr(2, packet.find('!') - 2);
	size_t start = packet.find("PRIVMSG");
	start += 8;
	size_t end = packet.find(' ', start);
	msg.channel = packet.substr(start, end - start);
	start = packet.find(':',end) + 1;
	msg.message = packet.substr(start);

	return (msg);
}

