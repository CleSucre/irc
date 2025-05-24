#include "Channel.hpp"
#include "JoinCommand.hpp"

CommandBase *getCommand(Client& client, const std::string& inputLine) {
	std::string line = inputLine;

	std::vector<std::string> tokens = split(line, ':');

	if (tokens.empty())
		return NULL;

	if (tokens[0][0] == ':' && tokens.size() >= 2)
		tokens.erase(tokens.begin());

	if (tokens[0] == "JOIN") {
		return new JoinCommand(client, tokens);
	}
	return NULL;
}

/**
 * @brief Handles the reception of packets from clients
 *
 * @param client The client that sent the packet
 * @param packet The packet received from the client
 */
void packetRecieption(Client& client, const std::string& packet) {
	std::cout << "Packet received from client " << client.getIp() << ": " << packet << std::endl;
	CommandBase *cmd = getCommand(client, packet);

	std::cout << "DEBUG 1" << std::endl;
	if (cmd) {
		std::cout << "DEBUG 2" << std::endl;
		std::string result = cmd->execute();
		if (!result.empty()) {
			client.sendMessage(result);
		}
		delete cmd;
	}
	//TODO: Handle messages?
}

// KICK		KICK <channel> <user> [:reason]
// INVITE	INVITE <user> <channel>
// MODE		MODE <channel> <flags> [args]
// TOPIC	TOPIC <channel> [:topic]

// JOIN		JOIN <channel> [key]
// PRIVMSG	PRIVMSG target [:message]
