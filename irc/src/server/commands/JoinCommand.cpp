#include "JoinCommand.hpp"
#include <set>

JoinCommand::JoinCommand(Client& client, const std::vector<std::string>& cmd)
	: CommandBase(client, cmd, true) {}

JoinCommand::~JoinCommand() {}

std::string JoinCommand::generateJoinResponse(Client* client, Channel* channel) {
	std::ostringstream response;

	response << ":" << client->getNick() << "!user@host JOIN :" << channel->getName();

	std::string topic = channel->getTopic();
	std::string serverName = client->getServer()->getName();
	if (!topic.empty())
		response << ":" << serverName << " 332 " << client->getNick() << " " << channel->getName()
		         << " :" << topic;
	else
		response << ":" << serverName << " 331 " << client->getNick() << " " << channel->getName()
		         << " :No topic is set";

	response << ":" << serverName << " 333 " << client->getNick() << " "
	         << channel->getName() << " " << client->getNick() << " " << generateTimestamp();

	std::string names = channel->getAllNicks();
	response << ":" << serverName << " 353 " << client->getNick() << " = "
	         << channel->getName() << " :" << names;

	response << ":" << serverName << " 366 " << client->getNick() << " "
	         << channel->getName() << " :End of /NAMES list.";

	return response.str();
}

Channel* JoinCommand::processChannel(const std::string& channelName, const std::string& key) {
	if (channelName.empty()) {
		_client.sendMessage(":" + _client.getServer()->getName() + " " + ERR_NOSUCHCHANNEL(_client.getNick(), channelName));
		return NULL;
	}

	if (channelName[0] != '#' && channelName[0] != '&') {
		_client.sendMessage(":" + _client.getServer()->getName() + " " + ERR_UNKNOWNCOMMAND(_client.getNick(), "JOIN"));
		return NULL;
	}

	Server* server = _client.getServer();
	Channel* channel = server->getChannelByName(channelName);

	bool newlyCreated = false;
	if (!channel) {
		channel = new Channel(channelName);
		server->addChannel(channel);
		channel->addAdmin(&_client);
		newlyCreated = true;
	}

	if (!newlyCreated && channel->getRole(&_client) >= user)
		return NULL;

	if (channel->getMode(mI) && !channel->isGuess(&_client)) {
		_client.sendMessage(":" + server->getName() + " " + ERR_INVITEONLYCHAN(_client.getNick(), channelName));
		return NULL;
	}

	if (channel->getMode(mK) && channel->getPassword() != key) {
		_client.sendMessage(":" + server->getName() + " " + ERR_BADCHANNELKEY(_client.getNick(), channelName));
		return NULL;
	}

	if (channel->getMode(mL) && channel->getSize() >= channel->getModeL()) {
		_client.sendMessage(":" + server->getName() + " " + ERR_CHANNELISFULL(_client.getNick(), channelName));
		return NULL;
	}

	if (!newlyCreated)
		channel->addUser(&_client);

	return channel;
}

/**
 * @brief JOIN <channel> [key]
 * 			JOIN <channel1>,<channel2>,... [key1,key2,...]
 */
void JoinCommand::execute() {
	Server* server = _client.getServer();
	std::string serverName = server->getName();

	if (_cmd.size() < 2) {
		_client.sendMessage(":" + serverName + " " + ERR_NEEDMOREPARAMS(_client.getNick(), "JOIN"));
		return;
	}

	std::vector<std::string> channels = split(getParameter(1), ',');
	std::vector<std::string> keys = split(getParameter(2), ',');

	for (size_t i = 0; i < channels.size(); ++i) {
		std::string chanName = channels[i];
		std::string key = (i < keys.size()) ? keys[i] : "";

		Channel* channel = processChannel(chanName, key);
		if (!channel)
			continue;

		std::string joinMsg = _client.getPrefix() + " JOIN :" + chanName;
		channel->broadcast(_client, joinMsg);
		_client.sendMessage(generateJoinResponse(&_client, channel));
	}
}
