#include "PartCommand.hpp"

PartCommand::PartCommand(Client& client, const std::vector<std::string>& cmd)
    : CommandBase(client, cmd, true) {}

PartCommand::~PartCommand() {}

/**
 * @brief PART #channel :Optional reason
 */
std::string PartCommand::execute() {
    Server *server = _client.getServer();
	std::string serverName = server->getName();
	if (_cmd.size() < 2) {
		_client.sendMessage(":" + serverName + " " + ERR_NEEDMOREPARAMS(_client.getNick(), "PART") + "\r\n");
		return "";
	}
	std::vector<std::string> channels = split(_cmd[1], ',');
	std::string reason = (_cmd.size() >= 3) ? joinFirstN(_cmd, 2) : " ";

	for (size_t i = 0; i < channels.size(); ++i) {
		Channel* channel = server->getChannelByName(channels[i]);
		if (!channel) {
			_client.sendMessage(":" + serverName + " " + ERR_NOSUCHCHANNEL(_client.getNick(), channels[i]) + "\r\n");
			continue;
		}
		if (channel->getRole(&_client) < 0) {
			_client.sendMessage(":" + serverName + " " + ERR_NOTONCHANNEL(_client.getNick(), channels[i]) + "\r\n");
			continue;
		}
		std::string msg = ":" + _client.getPrefix() + " PART " + channels[i] + "\r\n";
		if (!reason.empty()) msg += " :" + reason;
		channel->broadcast(_client, msg);
		channel->removeUser(&_client);
		if (channel->isEmpty()) server->removeChannel(channel);
	}
	return "";
}
