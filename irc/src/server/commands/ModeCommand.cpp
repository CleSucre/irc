#include "ModeCommand.hpp"

ModeCommand::ModeCommand(Client& client, const std::vector<std::string>& cmd)
    : CommandBase(client, cmd) {}

ModeCommand::~ModeCommand() {}

/**
 * @brief MODE <channel> <flags> [args]
 */
std::string ModeCommand::execute() {
	Server *server = _client.getServer();
	std::string serverName = server->getName();
	if (_cmd.size() < 2) {
		_client.sendMessage(":" + serverName + " " + ERR_NEEDMOREPARAMS(_client.getNick(), "MODE") + "\n");
		return "";
	}

	const std::string& target = _cmd[1];
	if (target[0] != '#') {
		_client.sendMessage(":" + serverName + " " + ERR_UNKNOWNCOMMAND(_client.getNick(), "MODE") + "\n");
		return "";
	}

	Channel* channel = server->getChannelByName(target);
	if (!channel) {
		_client.sendMessage(":" + serverName + " " + ERR_NOSUCHCHANNEL(_client.getNick(), target) + "\n");
		return "";
	}

	if (_cmd.size() == 2) {
		std::string modes = channel->getModeString(channel->isAdmin(&_client));
		_client.sendMessage(":" + serverName + " " + RPL_CHANNELMODEIS(_client.getNick(), target, modes) + "\n");
		return "";
	}

	if (channel->getRole(&_client) != admin) {
		_client.sendMessage(":" + serverName + " " + ERR_CHANOPRIVSNEEDED(_client.getNick(), target) + "\n");
		return "";
	}

	std::string flags = _cmd[2];
	bool adding = true;
	size_t argIndex = 3;

	for (size_t i = 0; i < flags.length(); ++i) {
		char c = flags[i];
		if (c == '+') {
			adding = true;
		} else if (c == '-') {
			adding = false;
		} else {
			switch (c) {
				case 'i': channel->setMode(mI, adding); break;
				case 't': channel->setMode(mT, adding); break;
				case 'k':
					if (adding) {
						if (argIndex >= _cmd.size()) {
							_client.sendMessage(":" + serverName + " " + ERR_NEEDMOREPARAMS(_client.getNick(), "MODE") + "\n");
							return "";
						}
						channel->setMode(mK, true);
						channel->setPassword(_cmd[argIndex++]);
					} else {
						channel->setMode(mK, false);
						channel->setPassword("");
					}
					break;
				case 'l':
					if (adding) {
						if (argIndex >= _cmd.size()) {
							_client.sendMessage(":" + serverName + " " + ERR_NEEDMOREPARAMS(_client.getNick(), "MODE") + "\n");
							return "";
						}
						channel->setMode(mL, true);
						channel->setModeL(std::atoi(_cmd[argIndex++].c_str()));
					} else {
						channel->setMode(mL, false);
					}
					break;
				case 'o':
					if (argIndex >= _cmd.size()) {
						_client.sendMessage(":" + serverName + " " + ERR_NEEDMOREPARAMS(_client.getNick(), "MODE") + "\n");
						return "";
					}
					{
						Client* targetClient = server->getClientByName(_cmd[argIndex++]);
						if (!targetClient || channel->getRole(targetClient) < 0) {
							_client.sendMessage(":" + serverName + " " + ERR_USERNOTINCHANNEL(_client.getNick(), _cmd[argIndex - 1], target) + "\n");
							return "";
						}
						if (adding)
							channel->addAdmin(targetClient);
						else
							channel->addUser(targetClient);  // à implémenter
					}
					break;
				default:
					_client.sendMessage(":" + serverName + " " + ERR_UNKNOWNMODE(_client.getNick(), std::string(1, c)) + "\n");
					return "";
			}
		}
	}

	std::string msg = ":" + _client.getPrefix() + " MODE " + target + " " + flags  + "\n";
	for (size_t i = 3; i < argIndex; ++i)
		msg += " " + _cmd[i];
	channel->broadcast(_client, msg);
	return "";
}
