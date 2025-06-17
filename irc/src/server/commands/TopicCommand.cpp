#include "TopicCommand.hpp"

TopicCommand::TopicCommand(Client& client, const std::vector<std::string>& cmd)
    : CommandBase(client, cmd, true) {}

TopicCommand::~TopicCommand() {}

/**
 * @brief TOPIC <channel> [:topic]
 */
void TopicCommand::execute() {
	Server *server = _client.getServer();
	std::string serverName = server->getName();
	if (_cmd.size() < 2) {
		_client.sendMessage(":" + serverName + " " + ERR_NEEDMOREPARAMS(_client.getNick(), "TOPIC"));
		return;
	}
	const std::string& channelName = getParameter(1);
	Channel* channel = server->getChannelByName(channelName);
	if (!channel) {
		_client.sendMessage(":" + serverName + " " + ERR_NOSUCHCHANNEL(_client.getNick(), channelName));
		return;
	}
	if (channel->getRole(&_client) < 0) {
		_client.sendMessage(":" + serverName + " " + ERR_NOTONCHANNEL(_client.getNick(), channelName));
		return;
	}
	if (_cmd.size() == 2) {
		std::string topic = channel->getTopic();
		if (topic.empty())
			_client.sendMessage(":" + serverName + " " + RPL_NOTOPIC(_client.getNick(), channelName));
		else
			_client.sendMessage(":" + serverName + " " + RPL_TOPIC(_client.getNick(), channelName, topic));
		return;
	}
	if (channel->getMode(mT) && channel->getRole(&_client) != admin) {
		_client.sendMessage(":" + serverName + " " + ERR_CHANOPRIVSNEEDED(_client.getNick(), channelName));
		return;
	}
	std::string newTopic = joinFirstN(std::vector<std::string>(_cmd.begin() + 2, _cmd.end()), _cmd.size());
	channel->setTopic(newTopic);
	channel->broadcast(_client, _client.getPrefix() + " TOPIC " + channelName + " :" + newTopic);
}
