#include "PrivmsgCommand.hpp"

PrivmsgCommand::PrivmsgCommand(Client& client, const std::vector<std::string>& cmd)
    : CommandBase(client, cmd) {}

PrivmsgCommand::~PrivmsgCommand() {}

/**
 * @brief PRIVMSG target [:message]
 */
std::string PrivmsgCommand::execute() {
    Server *server = _client.getServer();
	std::string serverName = server->getName();
	if (_cmd.size() < 2) {
		_client.sendMessage(":" + serverName + " " + ERR_NORECIPIENT(_client.getNick()) + "\n");
		return "";
	}
	if (_cmd.size() < 3) {
		_client.sendMessage(":" + serverName + " " + ERR_NOTEXTTOSEND(_client.getNick()) + "\n");
		return "";
	}
	const std::string& target = _cmd[1];
	std::string message = joinFirstN(_cmd, 2); // mettre tout les arg de _cmd en un

	if (target[0] == '#' || target[0] == '&') {
		Channel* channel = server->getChannelByName(target);
		if (!channel) {
			_client.sendMessage(":" + serverName + " " + ERR_NOSUCHCHANNEL(_client.getNick(), target) + "\n");
			return "";
		}
		if (channel->getRole(&_client) < 0) {
			_client.sendMessage(":" + serverName + " " + ERR_CANNOTSENDTOCHAN(_client.getNick(), target) + "\n");
			return "";
		}
		channel->broadcast(_client, ":" + _client.getPrefix() + " PRIVMSG " + target + " :" + message + "\n");
	} else {
		Client* recipient = server->getClientByName(target);
		if (!recipient) {
			_client.sendMessage(":" + serverName + " " + ERR_NOSUCHNICK(_client.getNick(), target) + "\n");
			return "";
		}
		recipient->sendMessage(":" + _client.getPrefix() + " PRIVMSG " + target + " :" + message + "\n");
	}
	return "";
}
