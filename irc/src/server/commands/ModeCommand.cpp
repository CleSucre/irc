#include "ModeCommand.hpp"

ModeCommand::ModeCommand(Client& client, const std::vector<std::string>& cmd)
    : CommandBase(client, cmd, true) {}

ModeCommand::~ModeCommand() {}

/**
 * @brief MODE <channel> <flags> [args]
 */
void ModeCommand::execute() {
	Server *server = _client.getServer();
	std::string serverName = server->getName();
	if (_cmd.size() < 2) {
		_client.sendMessage(":" + serverName + " " + ERR_NEEDMOREPARAMS(_client.getNick(), "MODE"));
		return;
	}

	const std::string& target = getParameter(1);
	if (target[0] != '#') {
		_client.sendMessage(":" + serverName + " " + ERR_UNKNOWNCOMMAND(_client.getNick(), "MODE"));
		return;
	}

	Channel* channel = server->getChannelByName(target);
	if (!channel) {
		_client.sendMessage(":" + serverName + " " + ERR_NOSUCHCHANNEL(_client.getNick(), target));
		return;
	}

	if (_cmd.size() == 2) {
		std::string modes = channel->getModeString(channel->isAdmin(&_client));
		_client.sendMessage(":" + serverName + " " + RPL_CHANNELMODEIS(_client.getNick(), target, modes));
		return;
	}

	if (_cmd.size() < 3 || getParameter(2).empty() || (getParameter(2)[0] != '+' && getParameter(2)[0] != '-')) {
		_client.sendMessage(":" + serverName + " " + ERR_NEEDMOREPARAMS(_client.getNick(), "MODE"));
		return;
	}

	if (channel->getRole(&_client) != admin) {
		_client.sendMessage(":" + serverName + " " + ERR_CHANOPRIVSNEEDED(_client.getNick(), target));
		return;
	}

	std::string flags = getParameter(2);
	bool adding = true;
	size_t argIndex = 3;

	std::string newFlags;
	std::vector<std::string> extraArgs;

	for (size_t i = 0; i < flags.length(); ++i) {
		char c = flags[i];
		if (c == '+') {
			adding = true;
			newFlags += '+';
		} else if (c == '-') {
			adding = false;
			newFlags += '-';
		} else {
			switch (c) {
				case 'i':
					channel->setMode(mI, adding);
					newFlags += 'i';
					break;
				case 't':
					channel->setMode(mT, adding);
					newFlags += 't';
					break;
				case 'k':
					if (adding) {
						if (argIndex >= _cmd.size()) {
							_client.sendMessage(":" + serverName + " " + ERR_NEEDMOREPARAMS(_client.getNick(), "MODE"));
							return;
						}
						channel->setMode(mK, true);
						channel->setPassword(getParameter(argIndex));
						extraArgs.push_back(getParameter(argIndex++));
						newFlags += 'k';
					} else {
						channel->setMode(mK, false);
						channel->setPassword("");
						newFlags += 'k';
					}
					break;
				case 'l':
					if (adding) {
						if (argIndex >= _cmd.size()) {
							_client.sendMessage(":" + serverName + " " + ERR_NEEDMOREPARAMS(_client.getNick(), "MODE"));
							return;
						}
						channel->setMode(mL, true);
						channel->setModeL(std::atoi(getParameter(argIndex).c_str()));
						extraArgs.push_back(getParameter(argIndex++));
						newFlags += 'l';
					} else {
						channel->setMode(mL, false);
						newFlags += 'l';
					}
					break;
				case 'o': {
					if (argIndex >= _cmd.size()) {
						_client.sendMessage(":" + serverName + " " + ERR_NEEDMOREPARAMS(_client.getNick(), "MODE"));
						return;
					}

					std::string targetNick = getParameter(argIndex++);
					Client* targetClient = server->getClientByNickname(targetNick);

					if (!targetClient || channel->getRole(targetClient) < user) {
						_client.sendMessage(":" + serverName + " " + ERR_USERNOTINCHANNEL(_client.getNick(), targetNick, target));
						return;
					}

					if (adding)
						channel->addAdmin(targetClient);
					else
						channel->addUser(targetClient);

					newFlags += 'o';
					extraArgs.push_back(targetNick);
					break;
				}
				default:
					_client.sendMessage(":" + serverName + " " + ERR_UNKNOWNMODE(_client.getNick(), std::string(1, c)));
					return;
			}
		}
	}

	std::string msg = _client.getPrefix() + " MODE " + target + " " + newFlags;
	for (size_t i = 0; i < extraArgs.size(); ++i)
		msg += " " + extraArgs[i];

	channel->broadcast(_client, msg);
	_client.sendMessage(msg);
}
