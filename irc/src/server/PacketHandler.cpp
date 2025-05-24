#include "irc.hpp"
#include "Channel.hpp"

std::vector<std::string> split(const std::string& line) {
	std::vector<std::string> tokens;
	std::istringstream iss(line);
	std::string word;

	while (iss >> word) {
		tokens.push_back(word);
		if (!word.empty() && word[0] == ':') {
			std::string rest;
			std::getline(iss, rest);
			tokens.back() += rest;
			break;
		}
	}
	return tokens;
}

std::vector<std::string> getCommand(const std::string& inputLine) {
	std::string line = inputLine;

	// if (!line.empty() && line.back() == '\n') line.pop_back();
	// if (!line.empty() && line.back() == '\r') line.pop_back();

	std::vector<std::string> tokens = split(line);

	if (tokens.empty())
		return tokens;

	if (tokens[0][0] == ':' && tokens.size() >= 2)
		tokens.erase(tokens.begin());

	return tokens;
}

/**
 * @brief Handles the reception of packets from clients
 *
 * @param client The client that sent the packet
 * @param packet The packet received from the client
 */
void packetRecieption(Client& client, const std::string& packet) {
	std::cout << "Packet received from client " << client.getFd() << ": " << packet << std::endl;
	std::vector<std::string> cmd = getCommand(packet);

	// traitement commande
	// if (cmd[1] == "KICK") {
	// 	kick(client, cmd);
	// } else if (cmd[1] == "INVITE") {
	// 	invite(client, cmd);
	// }else if (cmd[1] == "TOPIC") {
	// 	topic(client, cmd);
	// } else if (cmd[1] == "MODE") {
	// 	mode(client, cmd);
	// } else if (cmd[1] == "JOIN") {
	// 	join(client, cmd);
	// } else if (cmd[1] == "PRIVMSG") {
	// 	privmsg(client, cmd);
	// } else {
		// commande not found
	// }
}

// KICK		KICK <channel> <user> [:reason]
// INVITE	INVITE <user> <channel>
// MODE		MODE <channel> <flags> [args]
// TOPIC	TOPIC <channel> [:topic]

// JOIN		JOIN <channel> [key]
// PRIVMSG	PRIVMSG target [:message]
