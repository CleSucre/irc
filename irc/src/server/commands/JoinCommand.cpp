#include "JoinCommand.hpp"

JoinCommand::JoinCommand(Client& client, const std::vector<std::string>& cmd)
    : CommandBase(client, cmd, true) {}

JoinCommand::~JoinCommand() {}

/**
 * @brief JOIN <channel> [key]
 */
std::string JoinCommand::execute() {
    Server *server = this->getServer();
	std::string serverName = server->getName();
	if (_cmd.size() < 2) {
		_client.sendMessage(":" + serverName + " " + ERR_NEEDMOREPARAMS(_client.getNick(), "JOIN") + "\n");
		return "";
	}
	const std::string& channelName = _cmd[1];
	if (channelName[0] != '#' && channelName[0] != '&') {
		_client.sendMessage(":" + serverName + " " + ERR_UNKNOWNCOMMAND(_client.getNick(), "JOIN") + "\n");
		return "";
	}
	Channel* channel = server->getChannelByName(channelName);
	if (!channel) {
		channel = new Channel(channelName);
		channel->addAdmin(&_client);
		_client.sendMessage(":" + serverName + " " + RPL_NAMREPLY(_client.getNick(), channelName, channel->getAllNicks()) + "\n");
		_client.sendMessage(":" + serverName + " " + RPL_ENDOFNAMES(_client.getNick(), channelName) + "\n");
	}

	if (channel->getRole(&_client) >= 0)
		return "";

	if (channel->getMode(mI) && !channel->isGuess(&_client)) {
		_client.sendMessage(":" + serverName + " " + ERR_INVITEONLYCHAN(_client.getNick(), channelName) + "\n");
		return "";
	}

	if (channel->getMode(mK)) {
		if (_cmd.size() < 3 || channel->getPassword() != _cmd[2]) {
			_client.sendMessage(":" + serverName + " " + ERR_BADCHANNELKEY(_client.getNick(), channelName) + "\n");
			return "";
		}
	}

	if (channel->getMode(mL) && channel->getSize() >= channel->getModeL()) {
		_client.sendMessage(":" + serverName + " " + ERR_CHANNELISFULL(_client.getNick(), channelName) + "\n");
		return "";
	}

	channel->addUser(&_client);
	std::string joinMsg = ":" + _client.getPrefix() + " JOIN :" + channelName;
	channel->broadcast(_client, joinMsg);

	std::string topic = channel->getTopic();
	if (topic.empty())
		_client.sendMessage(":" + serverName + " " + RPL_NOTOPIC(_client.getNick(), channelName) + "\n");
	else
		_client.sendMessage(":" + serverName + " " + RPL_TOPIC(_client.getNick(), channelName, topic) + "\n");

	std::string names;
	std::vector<Client*> tmp = channel->getAdmin();

	for (size_t i = 0; i < tmp.size(); ++i)
		names += "@" + tmp[i]->getNick() + " ";

	tmp = channel->getUser();
	for (size_t i = 0; i < tmp.size(); ++i)
		names += tmp[i]->getNick() + " ";

	if (!names.empty() && names[names.size() - 1] == ' ')
		names[names.size() - 1] = '\0';

	_client.sendMessage(":" + serverName + " " + RPL_NAMREPLY(_client.getNick(), channelName, names) + "\n");
	_client.sendMessage(":" + serverName + " " + RPL_ENDOFNAMES(_client.getNick(), channelName) + "\n");
    return "";
}
