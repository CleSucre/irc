#include "PartCommand.hpp"

PartCommand::PartCommand(Client& client, const std::vector<std::string>& cmd)
	: CommandBase(client, cmd, true) {}

PartCommand::~PartCommand() {}

/**
 * @brief PART #channel :Optional reason
 */
void PartCommand::execute() {
	Server *server = _client.getServer();
	std::string serverName = server->getName();

	if (_cmd.size() < 2) {
		_client.sendMessage(":" + serverName + " " + ERR_NEEDMOREPARAMS(_client.getNick(), "PART"));
		return;
	}

	std::vector<std::string> channels = split(getParameter(1), ',');
	std::string reason = getParameter(2);

	for (size_t i = 0; i < channels.size(); ++i) {
		Channel* channel = server->getChannelByName(channels[i]);
		if (!channel) {
			_client.sendMessage(":" + serverName + " " + ERR_NOSUCHCHANNEL(_client.getNick(), channels[i]));
			continue;
		}

		if (channel->getRole(&_client) == guess) {
			_client.sendMessage(":" + serverName + " " + ERR_NOTONCHANNEL(_client.getNick(), channels[i]));
			continue;
		}

		std::string msg = _client.getPrefix() + " PART " + channel->getName();
		if (!reason.empty())
			msg += " :" + reason;

		channel->broadcast(_client, msg);
		_client.sendMessage(msg);
		channel->removeUser(&_client);

		if (channel->isEmpty())
			server->removeChannel(channel);
	}
}
