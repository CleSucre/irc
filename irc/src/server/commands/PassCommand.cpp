#include "PassCommand.hpp"

PassCommand::PassCommand(Client& client, const std::vector<std::string>& cmd)
    : CommandBase(client, cmd, false) {}

PassCommand::~PassCommand() {
	
}

/**
 * @brief PASS <password>
 */
std::string PassCommand::execute() {
	Server *server = _client.getServer();
	std::string serverName = server->getName();

	if (_cmd.size() < 2) {
		return ERR_NEEDMOREPARAMS(_client.getPrefix(), "PASS");
	}

	std::string password = this->getParameter(1);

	if (!server->checkPassword(password)) {
		return ERR_PASSWDMISMATCH(_client.getPrefix());
	}
	_client.setValidPassword();
	getClient().checkIdentification();
	return "";
}