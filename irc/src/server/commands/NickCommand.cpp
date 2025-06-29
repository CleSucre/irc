#include "NickCommand.hpp"

NickCommand::NickCommand(Client& client, const std::vector<std::string>& cmd)
    : CommandBase(client, cmd, false) {}

NickCommand::~NickCommand() {}

/**
 * @brief NICK <nickname>
 */
void NickCommand::execute() {
    Server *server = _client.getServer();
	std::string serverName = server->getName();

	if (_cmd.size() != 2) {
		_client.sendMessage(":" + serverName + " " + ERR_NEEDMOREPARAMS(_client.getPrefix(), "NICK"));
		return;
	}

	std::string newNick = this->getParameter(1);

	int result = _client.setNick(newNick);
	if (result == 0) {
		_client.sendMessage(":" + serverName + " " + ERR_ERRONEUSNICKNAME(_client.getPrefix(), newNick));
		return;
	} else if (result == -1) {
		_client.sendMessage(":" + serverName + " " + ERR_NICKNAMEINUSE(newNick));
		return;
	}
	getClient().checkIdentification();
}
