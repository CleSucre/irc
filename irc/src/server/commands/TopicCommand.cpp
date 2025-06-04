#include "TopicCommand.hpp"

TopicCommand::TopicCommand(Client& client, const std::vector<std::string>& cmd)
    : CommandBase(client, cmd) {}

TopicCommand::~TopicCommand() {}

/**
 * @brief TOPIC <channel> [:topic]
 */
std::string TopicCommand::execute() {
	Server *server = _client.getServer();
	std::string serverName = server->getName();
	if (_cmd.size() < 2) {
		_client.sendMessage(":" + serverName + " " + ERR_NEEDMOREPARAMS(_client.getNick(), "TOPIC") + "\n");
		return "";
	}
	const std::string& channelName = _cmd[1];
	Channel* channel = server->getChannelByName(channelName);
	if (!channel) {
		_client.sendMessage(":" + serverName + " " + ERR_NOSUCHCHANNEL(_client.getNick(), channelName) + "\n");
		return "";
	}
	if (channel->getRole(&_client) < 0) {
		_client.sendMessage(":" + serverName + " " + ERR_NOTONCHANNEL(_client.getNick(), channelName) + "\n");
		return "";
	}
	if (_cmd.size() == 2) {
		std::string topic = channel->getTopic();
		if (topic.empty())
			_client.sendMessage(":" + serverName + " " + RPL_NOTOPIC(_client.getNick(), channelName) + "\n");
		else
			_client.sendMessage(":" + serverName + " " + RPL_TOPIC(_client.getNick(), channelName, topic) + "\n");
		return "";
	}
	if (channel->getMode(mT) && channel->getRole(&_client) != admin) {
		_client.sendMessage(":" + serverName + " " + ERR_CHANOPRIVSNEEDED(_client.getNick(), channelName) + "\n");
		return "";
	}
	std::string newTopic = joinFirstN(_cmd, 2);
	channel->setTopic(newTopic);
	channel->broadcast(_client, ":" + _client.getPrefix() + " TOPIC " + channelName + " :" + newTopic + "\n");
    return "";
}
