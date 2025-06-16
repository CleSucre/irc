#include "PrivmsgCommand.hpp"

PrivmsgCommand::PrivmsgCommand(Client& client, const std::vector<std::string>& cmd)
	: CommandBase(client, cmd, true) {}

PrivmsgCommand::~PrivmsgCommand() {}

/**
 * @brief PRIVMSG target [:message]
 * 		if target is a channel, replace target by: #channel
 * 		if target is a user, replace target by: username
 */
void PrivmsgCommand::execute() {
	Server *server = _client.getServer();
	std::string serverName = server->getName();
	if (_cmd.size() < 2) {
		_client.sendMessage(":" + serverName + " " + ERR_NORECIPIENT(_client.getNick()));
		return;
	}
	if (_cmd.size() < 3) {
		_client.sendMessage(":" + serverName + " " + ERR_NOTEXTTOSEND(_client.getNick()));
		return;
	}

	const std::string& target = _cmd[1];
	std::string message = joinFirstN(std::vector<std::string>(_cmd.begin() + 2, _cmd.end()), _cmd.size());

	if (target[0] == '#') {
		Channel* channel = server->getChannelByName(target);
		if (!channel) {
			_client.sendMessage(":" + serverName + " " + ERR_NOSUCHCHANNEL(_client.getNick(), target) + "\r\n");
			return ;
		}
		if (channel->getRole(&_client) < 0) {
			_client.sendMessage(":" + serverName + " " + ERR_CANNOTSENDTOCHAN(_client.getNick(), target) + "\r\n");
			return ;
		}
		channel->broadcast(_client, _client.getPrefix() + " PRIVMSG " + target + " :" + message + "\r\n");
	}
	else {
		Client* recipient = server->getClientByName(target);
		if (!recipient) {
			_client.sendMessage(":" + serverName + " " + ERR_NOSUCHNICK(_client.getNick(), target) + "\r\n");
			return ;
		}
		recipient->sendMessage(_client.getPrefix() + " PRIVMSG " + target + " :" + message + "\r\n");
	}
	return ;
}
