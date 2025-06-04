#include "WhoCommand.hpp"

WhoCommand::WhoCommand(Client& client, const std::vector<std::string>& cmd)
	: CommandBase(client, cmd) {}

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
			std::string gecos = "";

			_client.sendMessage(":" + serverName + " 352 " + _client.getNick() + " * " + target->getUser() \
			 + " " + target->getIp() + " " + serverName+ " " + target->getNick() + " H :0 " + gecos + "\n");
		}

		_client.sendMessage(":" + serverName + " 315 " + _client.getNick() + " * :End of /WHO list\n");
		return "";
	}
	// if (_cmd.size() >= 2) {
	// 	const std::string& arg = _cmd[1];

	// 	Channel* channel = server->getChannelByName(arg);
	// 	if (channel) {
	// 		std::vector<Client*> members = channel->getAllClients();

	// 		for (size_t i = 0; i < members.size(); ++i) {
	// 			Client* target = members[i];
	// 			std::string flag = "";
	// 			if (channel->isAdmin(target))
	// 				flag += "@";

	// 			std::string gecos = "";

	// 			_client.sendMessage(":" + serverName + " 352 " + _client.getNick() + " " + arg + " " + target->getUser()
	// 			+ " " + target->getIp() + " " + serverName + " " + target->getNick() + " H" + flag + " :0 " + gecos);
	// 		}

	// 		_client.sendMessage(":" + serverName + " 315 " + _client.getNick() + " " + arg + " :End of /WHO list");
	// 		return "";
	// 	}
	// }

	return "";
}
