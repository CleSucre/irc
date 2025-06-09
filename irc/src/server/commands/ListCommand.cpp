#include "ListCommand.hpp"

ListCommand::ListCommand(Client& client, const std::vector<std::string>& cmd)
	: CommandBase(client, cmd, true) {}

ListCommand::~ListCommand() {}

/**
 * @brief LIST [<channel>{,<channel>}]
 */
std::string ListCommand::execute() {
	Server *server = _client.getServer();
	std::string serverName = server->getName();
	if (_cmd.size() == 1) {
		std::vector<Channel *> channels = server->getAllChannels();

		_client.sendMessage(":" + serverName + " " + RPL_LISTSTART(_client.getNick()) + "\r\n");

		for (size_t i = 0; i < channels.size(); ++i) {
			Channel* channel = channels[i];
			std::string name = channel->getName();
			unsigned int nbrUser = channel->getSize();
			std::ostringstream oss;
			oss << nbrUser;
			std::string strUser = oss.str();
			std::string topic = channel->getTopic();

			_client.sendMessage(":" + serverName + " " + RPL_LIST(_client.getNick(), name, strUser, topic) + "\r\n");
		}

		_client.sendMessage(":" + serverName + " " + RPL_LISTEND(_client.getNick()) + "\r\n");
		return "";
	}

	_client.sendMessage(":" + serverName + " " + RPL_LISTSTART(_client.getNick()) + "\r\n");
	for (size_t i = 1; i < _cmd.size(); ++i) {
		const std::string& channelName = _cmd[i];
		Channel* channel = server->getChannelByName(channelName);

		if (!channel) {
			_client.sendMessage(":" + serverName + " " + ERR_NOSUCHCHANNEL(_client.getNick(), channelName) + "\r\n");
			continue;
		}

		std::string name = channel->getName();
		unsigned int nbrUser = channel->getSize();

		std::ostringstream oss;
		oss << nbrUser;
		std::string strNbrUser = oss.str();

		std::string topic = channel->getTopic();

		_client.sendMessage(":" + serverName + " " + RPL_LIST(_client.getNick(), name, strNbrUser, topic) + "\r\n");
	}

	_client.sendMessage(":" + serverName + " " + RPL_LISTEND(_client.getNick()) + "\r\n");
	return "";
}
