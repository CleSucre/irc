#include "JoinCommand.hpp"
#include <set>

JoinCommand::JoinCommand(Client& client, const std::vector<std::string>& cmd)
	: CommandBase(client, cmd, true) {}

JoinCommand::~JoinCommand() {}

/**
 * @brief JOIN <channel> [key]
 */
void JoinCommand::execute() {
	Server *server = this->getServer();
	std::string serverName = server->getName();

	if (_cmd.size() < 2) {
		_client.sendMessage(":" + serverName + " " + ERR_NEEDMOREPARAMS(_client.getNick(), "JOIN") + "\r\n");
		return;
	}

	const std::string& channelName = _cmd[1];

	if (channelName[0] != '#' && channelName[0] != '&') {
		_client.sendMessage(":" + serverName + " " + ERR_UNKNOWNCOMMAND(_client.getNick(), "JOIN") + "\r\n");
		return;
	}

	Channel* channel = server->getChannelByName(channelName);
	if (!channel) {
		channel = new Channel(channelName);
		server->addChannel(channel);
		channel->addAdmin(&_client);
	}

	if (channel->getRole(&_client) >= user) {
		return;
	}

	if (channel->getMode(mI) && !channel->isGuess(&_client)) {
		_client.sendMessage(":" + serverName + " " + ERR_INVITEONLYCHAN(_client.getNick(), channelName) + "\r\n");
		return;
	}

	if (channel->getMode(mK)) {
		if (_cmd.size() < 3 || channel->getPassword() != _cmd[2])
			_client.sendMessage(":" + serverName + " " + ERR_BADCHANNELKEY(_client.getNick(), channelName) + "\r\n");
		return;
	}

	if (channel->getMode(mL) && channel->getSize() >= channel->getModeL()) {
		_client.sendMessage(":" + serverName + " " + ERR_CHANNELISFULL(_client.getNick(), channelName) + "\r\n");
		return;
	}

	channel->addUser(&_client);

	std::string joinMsg = ":" + _client.getPrefix() + " JOIN :" + channelName + "\r\n";
	channel->broadcast(_client, joinMsg);

	std::string topic = channel->getTopic();
	if (topic.empty())
		_client.sendMessage(":" + serverName + " " + RPL_NOTOPIC(_client.getNick(), channelName) + "\r\n");
	else
		_client.sendMessage(":" + serverName + " " + RPL_TOPIC(_client.getNick(), channelName, topic) + "\r\n");

	std::string names;
	std::vector<Client*> admins = channel->getAdmin();
	std::vector<Client*> users = channel->getUser();

	std::set<std::string> already;

	for (size_t i = 0; i < admins.size(); ++i) {
		names += "@" + admins[i]->getNick() + " ";
		already.insert(admins[i]->getNick());
	}

	for (size_t i = 0; i < users.size(); ++i) {
		if (already.find(users[i]->getNick()) == already.end()) {
			names += users[i]->getNick() + " ";
		}
	}

	if (!names.empty() && names[names.size() - 1] == ' ')
		names.erase(names.size() - 1);

	_client.sendMessage(":" + serverName + " " + RPL_NAMREPLY(_client.getNick(), channelName, names) + "\r\n");
	_client.sendMessage(":" + serverName + " " + RPL_ENDOFNAMES(_client.getNick(), channelName) + "\r\n");
}
