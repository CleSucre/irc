#include "InviteCommand.hpp"

InviteCommand::InviteCommand(Client& client, const std::vector<std::string>& cmd)
    : CommandBase(client, cmd, true) {}

InviteCommand::~InviteCommand() {}

/**
 * @brief INVITE <user> <channel>
 */
std::string InviteCommand::execute() {

    Server *server = _client.getServer();
	std::string serverName = server->getName();
	if (_cmd.size() < 3) {
		_client.sendMessage(":" + serverName + " " + ERR_NEEDMOREPARAMS(_client.getNick(), "INVITE") + "\r\n");
		return "";
	}

	const std::string& toInviteNick = _cmd[1];
	const std::string& channelName = _cmd[2];

	Channel* channel = server->getChannelByName(channelName);
	if (!channel) {
		_client.sendMessage(":" + serverName + " " + ERR_NOSUCHCHANNEL(_client.getNick(), channelName) + "\r\n");
		return "";
	}

	Client* toInvite = server->getClientByName(toInviteNick);
	if (!toInvite) {
		_client.sendMessage(":" + serverName + " " + ERR_NOSUCHNICK(_client.getNick(), toInviteNick) + "\r\n");
		return "";
	}

	int result = channel->inviteUser(&_client, toInvite);
	switch (result) {
		case 0:
			toInvite->sendMessage(":" + _client.getNick() + " INVITE " + toInviteNick + " " + channelName);
			break;
		case 1:
			_client.sendMessage(":" + serverName + " " + ERR_CHANOPRIVSNEEDED(_client.getNick(), channelName) + "\r\n");
			break;
		case 2:
			_client.sendMessage(":" + serverName + " " + ERR_NOTONCHANNEL(_client.getNick(), channelName) + "\r\n");
			break;
		case 3:
			_client.sendMessage(":" + serverName + " " + ERR_USERONCHANNEL(_client.getNick(), toInviteNick, channelName) + "\r\n");
			break;
	}
	return "";
}
