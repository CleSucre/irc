/* made by tao_la_puissance (dev trainee)*/

#include "Channel.hpp"

// KICK		KICK <channel> <user> [:reason]
void kick(Client* client, const std::vector<std::string>& cmd) {
	Server *server = client->getServer();
	std::string serverName = server->getName();
	if (cmd.size() < 3) {
		client->sendMessage(":" + serverName + " " + ERR_NEEDMOREPARAMS(client->getNick(), "KICK"));
		return;
	}
	Channel* channel = server->getChannelByName(cmd[1]);
	if (!channel) {
		client->sendMessage(":" + serverName + " " + ERR_NOSUCHCHANNEL(client->getNick(), cmd[1]));
		return;
	}
	Client* toKick = server->getClientByName(cmd[2]);
	if (!toKick) {
		client->sendMessage(":" + serverName + " " + ERR_NOSUCHNICK(client->getNick(), cmd[2]));
		return;
	}
	int result = channel->kickUser(client, toKick);
	switch (result) {
		case 0: {
			std::string msg = ":" + client->getPrefix() + " KICK " + channel->getName() + " " + toKick->getNick();
			if (cmd.size() > 3) msg += " :" + joinFirstN(cmd, 3); // mettre tout les arg de cmd en un
			channel->broadcast(*client, msg);
			break;
		}
		case 1:
			client->sendMessage(":" + serverName + " " + ERR_CHANOPRIVSNEEDED(client->getNick(), channel->getName()));
			break;
		case 2:
			client->sendMessage(":" + serverName + " " + ERR_NOTONCHANNEL(client->getNick(), channel->getName()));
			break;
		case 3:
			client->sendMessage(":" + serverName + " " + ERR_USERNOTINCHANNEL(client->getNick(), toKick->getNick(), channel->getName()));
			break;
	}
}


// INVITE	INVITE <user> <channel>
void invite(Client* client, const std::vector<std::string>& cmd) {
	Server *server = client->getServer();
	std::string serverName = server->getName();
	if (cmd.size() < 3) {
		client->sendMessage(":" + serverName + " " + ERR_NEEDMOREPARAMS(client->getNick(), "INVITE"));
		return;
	}

	const std::string& toInviteNick = cmd[1];
	const std::string& channelName = cmd[2];

	Channel* channel = server->getChannelByName(channelName);
	if (!channel) {
		client->sendMessage(":" + serverName + " " + ERR_NOSUCHCHANNEL(client->getNick(), channelName));
		return;
	}

	Client* toInvite = server->getClientByName(toInviteNick);
	if (!toInvite) {
		client->sendMessage(":" + serverName + " " + ERR_NOSUCHNICK(client->getNick(), toInviteNick));
		return;
	}

	int result = channel->inviteUser(client, toInvite);
	switch (result) {
		case 0:
			toInvite->sendMessage(":" + client->getNick() + " INVITE " + toInviteNick + " " + channelName);
			break;
		case 1:
			client->sendMessage(":" + serverName + " " + ERR_CHANOPRIVSNEEDED(client->getNick(), channelName));
			break;
		case 2:
			client->sendMessage(":" + serverName + " " + ERR_NOTONCHANNEL(client->getNick(), channelName));
			break;
		case 3:
			client->sendMessage(":" + serverName + " " + ERR_USERONCHANNEL(client->getNick(), toInviteNick, channelName));
			break;
	}
}


// TOPIC	TOPIC <channel> [:topic]
void topic(Client* client, const std::vector<std::string>& cmd) {
	Server *server = client->getServer();
	std::string serverName = server->getName();
	if (cmd.size() < 2) {
		client->sendMessage(":" + serverName + " " + ERR_NEEDMOREPARAMS(client->getNick(), "TOPIC"));
		return;
	}
	const std::string& channelName = cmd[1];
	Channel* channel = server->getChannelByName(channelName);
	if (!channel) {
		client->sendMessage(":" + serverName + " " + ERR_NOSUCHCHANNEL(client->getNick(), channelName));
		return;
	}
	if (channel->getRole(client) < 0) {
		client->sendMessage(":" + serverName + " " + ERR_NOTONCHANNEL(client->getNick(), channelName));
		return;
	}
	if (cmd.size() == 2) {
		std::string topic = channel->getTopic();
		if (topic.empty())
			client->sendMessage(":" + serverName + " " + RPL_NOTOPIC(client->getNick(), channelName));
		else
			client->sendMessage(":" + serverName + " " + RPL_TOPIC(client->getNick(), channelName, topic));
		return;
	}
	if (channel->getMode(mT) && channel->getRole(client) != admin) {
		client->sendMessage(":" + serverName + " " + ERR_CHANOPRIVSNEEDED(client->getNick(), channelName));
		return;
	}
	std::string newTopic = joinFirstN(cmd, 2); // mettre tout les arg de cmd en un
	channel->setTopic(newTopic);
	channel->broadcast(*client, ":" + client->getPrefix() + " TOPIC " + channelName + " :" + newTopic);
}

// MODE		MODE <channel> <flags> [args]
void mode(Client* client, const std::vector<std::string>& cmd) {
	Server *server = client->getServer();
	std::string serverName = server->getName();
	if (cmd.size() < 2) {
		client->sendMessage(":" + serverName + " " + ERR_NEEDMOREPARAMS(client->getNick(), "MODE"));
		return;
	}

	const std::string& target = cmd[1];
	if (target[0] != '#' && target[0] != '&') {
		client->sendMessage(":" + serverName + " " + ERR_UNKNOWNCOMMAND(client->getNick(), "MODE"));
		return;
	}

	Channel* channel = server->getChannelByName(target);
	if (!channel) {
		client->sendMessage(":" + serverName + " " + ERR_NOSUCHCHANNEL(client->getNick(), target));
		return;
	}

	if (cmd.size() == 2) {
		std::string modes = channel->getModeString(channel->isAdmin(client));
		client->sendMessage(":" + serverName + " " + RPL_CHANNELMODEIS(client->getNick(), target, modes));
		return;
	}

	if (channel->getRole(client) != admin) {
		client->sendMessage(":" + serverName + " " + ERR_CHANOPRIVSNEEDED(client->getNick(), target));
		return;
	}

	std::string flags = cmd[2];
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
						if (argIndex >= cmd.size()) {
							client->sendMessage(":" + serverName + " " + ERR_NEEDMOREPARAMS(client->getNick(), "MODE"));
							return;
						}
						channel->setMode(mK, true);
						channel->setPassword(cmd[argIndex++]);
					} else {
						channel->setMode(mK, false);
						channel->setPassword("");
					}
					break;
				case 'l':
					if (adding) {
						if (argIndex >= cmd.size()) {
							client->sendMessage(":" + serverName + " " + ERR_NEEDMOREPARAMS(client->getNick(), "MODE"));
							return;
						}
						channel->setMode(mL, true);
						channel->setModeL(std::atoi(cmd[argIndex++].c_str()));
					} else {
						channel->setMode(mL, false);
					}
					break;
				case 'o':
					if (argIndex >= cmd.size()) {
						client->sendMessage(":" + serverName + " " + ERR_NEEDMOREPARAMS(client->getNick(), "MODE"));
						return;
					}
					{
						Client* targetClient = server->getClientByName(cmd[argIndex++]);
						if (!targetClient || channel->getRole(targetClient) < 0) {
							client->sendMessage(":" + serverName + " " + ERR_USERNOTINCHANNEL(client->getNick(), cmd[argIndex - 1], target));
							return;
						}
						if (adding)
							channel->addAdmin(targetClient);
						else
							channel->addUser(targetClient);  // à implémenter
					}
					break;
				default:
					client->sendMessage(":" + serverName + " " + ERR_UNKNOWNMODE(client->getNick(), std::string(1, c)));
					return;
			}
		}
	}

	std::string msg = ":" + client->getPrefix() + " MODE " + target + " " + flags;
	for (size_t i = 3; i < argIndex; ++i)
		msg += " " + cmd[i];
	channel->broadcast(*client, msg);
}



// JOIN		JOIN <channel> [key]
void join(Client* client, const std::vector<std::string>& cmd) {
	Server *server = client->getServer();
	std::string serverName = server->getName();
	if (cmd.size() < 2) {
		client->sendMessage(":" + serverName + " " + ERR_NEEDMOREPARAMS(client->getNick(), "JOIN"));
		return;
	}
	const std::string& channelName = cmd[1];
	if (channelName[0] != '#' && channelName[0] != '&') {
		client->sendMessage(":" + serverName + " " + ERR_UNKNOWNCOMMAND(client->getNick(), "JOIN"));
		return;
	}
	Channel* channel = server->getChannelByName(channelName);
	if (!channel) {
		channel = new Channel(channelName);
		channel->addAdmin(client);
		client->sendMessage(":" + serverName + " " + RPL_NAMREPLY(client->getNick(), channelName, channel->getAllNicks()));
		client->sendMessage(":" + serverName + " " + RPL_ENDOFNAMES(client->getNick(), channelName));
	}

	if (channel->getRole(client) >= 0)
		return;

	if (channel->getMode(mI) && !channel->isGuess(client)) {
		client->sendMessage(":" + serverName + " " + ERR_INVITEONLYCHAN(client->getNick(), channelName));
		return;
	}

	if (channel->getMode(mK)) {
		if (cmd.size() < 3 || channel->getPassword() != cmd[2]) {
			client->sendMessage(":" + serverName + " " + ERR_BADCHANNELKEY(client->getNick(), channelName));
			return;
		}
	}

	if (channel->getMode(mL) && channel->getSize() >= channel->getModeL()) {
		client->sendMessage(":" + serverName + " " + ERR_CHANNELISFULL(client->getNick(), channelName));
		return;
	}

	channel->addUser(client);
	std::string joinMsg = ":" + client->getPrefix() + " JOIN :" + channelName;
	channel->broadcast(*client, joinMsg);

	std::string topic = channel->getTopic();
	if (topic.empty())
		client->sendMessage(":" + serverName + " " + RPL_NOTOPIC(client->getNick(), channelName));
	else
		client->sendMessage(":" + serverName + " " + RPL_TOPIC(client->getNick(), channelName, topic));

	std::string names;
	std::vector<Client*> tmp = channel->getAdmin();

	for (size_t i = 0; i < tmp.size(); ++i)
		names += "@" + tmp[i]->getNick() + " ";

	tmp = channel->getUser();
	for (size_t i = 0; i < tmp.size(); ++i)
		names += tmp[i]->getNick() + " ";

	if (!names.empty() && names[names.size() - 1] == ' ')
		names[names.size() - 1] = '\0';

	client->sendMessage(":" + serverName + " " + RPL_NAMREPLY(client->getNick(), channelName, names));
	client->sendMessage(":" + serverName + " " + RPL_ENDOFNAMES(client->getNick(), channelName));
}



// PRIVMSG	PRIVMSG target [:message]
void privmsg(Client* client, const std::vector<std::string>& cmd) {
	Server *server = client->getServer();
	std::string serverName = server->getName();
	if (cmd.size() < 2) {
		client->sendMessage(":" + serverName + " " + ERR_NORECIPIENT(client->getNick()));
		return;
	}
	if (cmd.size() < 3) {
		client->sendMessage(":" + serverName + " " + ERR_NOTEXTTOSEND(client->getNick()));
		return;
	}
	const std::string& target = cmd[1];
	std::string message = joinFirstN(cmd, 2); // mettre tout les arg de cmd en un

	if (target[0] == '#' || target[0] == '&') {
		Channel* channel = server->getChannelByName(target);
		if (!channel) {
			client->sendMessage(":" + serverName + " " + ERR_NOSUCHCHANNEL(client->getNick(), target));
			return;
		}
		if (channel->getRole(client) < 0) {
			client->sendMessage(":" + serverName + " " + ERR_CANNOTSENDTOCHAN(client->getNick(), target));
			return;
		}
		channel->broadcast(*client, ":" + client->getPrefix() + " PRIVMSG " + target + " :" + message);
	} else {
		Client* recipient = server->getClientByName(target);
		if (!recipient) {
			client->sendMessage(":" + serverName + " " + ERR_NOSUCHNICK(client->getNick(), target));
			return;
		}
		recipient->sendMessage(":" + client->getPrefix() + " PRIVMSG " + target + " :" + message);
	}
}


// PART #channel :Optional reason
void part(Client* client, const std::vector<std::string>& cmd) {
	Server *server = client->getServer();
	std::string serverName = server->getName();
	if (cmd.size() < 2) {
		client->sendMessage(":" + serverName + " " + ERR_NEEDMOREPARAMS(client->getNick(), "PART"));
		return;
	}
	std::vector<std::string> channels = split(cmd[1], ',');
	std::string reason = (cmd.size() >= 3) ? joinFirstN(cmd, 2) : " "; // mettre tout les arg de cmd en un: ""

	for (size_t i = 0; i < channels.size(); ++i) {
		Channel* channel = server->getChannelByName(channels[i]);
		if (!channel) {
			client->sendMessage(":" + serverName + " " + ERR_NOSUCHCHANNEL(client->getNick(), channels[i]));
			continue;
		}
		if (channel->getRole(client) < 0) {
			client->sendMessage(":" + serverName + " " + ERR_NOTONCHANNEL(client->getNick(), channels[i]));
			continue;
		}
		std::string msg = ":" + client->getPrefix() + " PART " + channels[i];
		if (!reason.empty()) msg += " :" + reason;
		channel->broadcast(*client, msg);
		channel->removeUser(client);
		if (channel->isEmpty()) server->removeChannel(channel);
	}
}
