#include "KickCommand.hpp"

KickCommand::KickCommand(Client& client, const std::vector<std::string>& cmd)
    : CommandBase(client, cmd) {}

KickCommand::~KickCommand() {}

/**
 * @brief KICK <channel> <user> [:reason]
 */
std::string KickCommand::execute() {
    Server *server = _client.getServer();
	std::string serverName = server->getName();
	if (_cmd.size() < 3) {
		_client.sendMessage(":" + serverName + " " + ERR_NEEDMOREPARAMS(_client.getNick(), "KICK"));
		return "";
	}
	Channel* channel = server->getChannelByName(_cmd[1]);
	if (!channel) {
		_client.sendMessage(":" + serverName + " " + ERR_NOSUCHCHANNEL(_client.getNick(), _cmd[1]));
		return "";
	}
	Client* toKick = server->getClientByName(_cmd[2]);
	if (!toKick) {
		_client.sendMessage(":" + serverName + " " + ERR_NOSUCHNICK(_client.getNick(), _cmd[2]));
		return "";
	}
	int result = channel->kickUser(&_client, toKick);
	switch (result) {
		case 0: {
			std::string msg = ":" + _client.getPrefix() + " KICK " + channel->getName() + " " + toKick->getNick();
			if (_cmd.size() > 3) msg += " :" + joinFirstN(_cmd, 3); // mettre tout les arg de _cmd en un
			channel->broadcast(_client, msg);
			break;
		}
		case 1:
			_client.sendMessage(":" + serverName + " " + ERR_CHANOPRIVSNEEDED(_client.getNick(), channel->getName()));
			break;
		case 2:
			_client.sendMessage(":" + serverName + " " + ERR_NOTONCHANNEL(_client.getNick(), channel->getName()));
			break;
		case 3:
			_client.sendMessage(":" + serverName + " " + ERR_USERNOTINCHANNEL(_client.getNick(), toKick->getNick(), channel->getName()));
			break;
	}
	return "";
}