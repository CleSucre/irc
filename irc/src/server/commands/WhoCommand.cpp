#include "WhoCommand.hpp"

WhoCommand::WhoCommand(Client& client, const std::vector<std::string>& cmd)
	: CommandBase(client, cmd, true) {}

WhoCommand::~WhoCommand() {}

/**
 * @brief WHO [<mask> ["o"]]
 */
std::string WhoCommand::execute() {
	Server *server = _client.getServer();
	std::string serverName = server->getName();
	if (_cmd.size() == 1) {
		std::vector<Client*> clients = server->getAllClients();

		for (size_t i = 0; i < clients.size(); ++i) {
			Client* target = clients[i];

			_client.sendMessage(":" + serverName + " 352 " + _client.getNick() + " * " + target->getNick() + " " \
			 + target->getIp() + " " + serverName+ " " + target->getNick() + " H :0 " + target->getUser() + "\r\n");
		}

		_client.sendMessage(":" + serverName + " 315 " + _client.getNick() + " * :End of /WHO list" + "\r\n");
		return "";
	}

	if (_cmd.size() > 2) {
		_client.sendMessage(":" + serverName + " " + ERR_NEEDMOREPARAMS(_client.getNick(), "WHO") + "\r\n");
		return "";
	}

	const std::string& channelName = _cmd[1];
	Channel* channel = server->getChannelByName(channelName);
	if (!channel) {
		_client.sendMessage(":" + serverName + " " + ERR_NOSUCHCHANNEL(_client.getNick(), channelName) + "\r\n");
		return "";
	}

	std::vector<Client*> Admin = channel->getAdmin();

	for (size_t i = 0; i < Admin.size(); ++i) {
		Client* target = Admin[i];
		_client.sendMessage(":" + serverName + " 352 " + _client.getNick() + " " + channelName + " " + target->getNick() \
		 + " " + target->getIp() + " " + serverName + " " + target->getNick() + " H@ :0 " + target->getUser() + "\r\n");
	}

	std::vector<Client*> User = channel->getUser();

	for (size_t i = 0; i < User.size(); ++i) {
		Client* target = User[i];
		_client.sendMessage(":" + serverName + " 352 " + _client.getNick() + " " + channelName + " " + target->getNick() \
		 + " " + target->getIp() + " " + serverName + " " + target->getNick() + " H :0 " + target->getUser() + "\r\n");
	}

	_client.sendMessage(":" + serverName + " 315 " + _client.getNick() + " " + channelName + " :End of /WHO list" + "\r\n");

	return "";
}
