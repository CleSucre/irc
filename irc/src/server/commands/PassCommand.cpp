#include "PassCommand.hpp"

PassCommand::PassCommand(Client& client, const std::vector<std::string>& cmd)
    : CommandBase(client, cmd, false) {}

PassCommand::~PassCommand() {

}

/**
 * @brief PASS <password>
 */
void PassCommand::execute() {
	Server *server = _client.getServer();
	std::string serverName = server->getName();

	if (_cmd.size() < 2) {
		_client.sendMessage(":" + serverName + " " + ERR_NEEDMOREPARAMS(_client.getPrefix(), "PASS"));
		return;
	}

	std::string password = this->getParameter(1);

	if (!server->checkPassword(password)) {
		_client.sendMessage(":" + serverName + " " + ERR_NEEDMOREPARAMS(_client.getPrefix(), "PASS"));
		return;
	}
	_client.setValidPassword();
	getClient().checkIdentification();
}
