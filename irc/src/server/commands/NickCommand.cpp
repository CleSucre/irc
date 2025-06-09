#include "NickCommand.hpp"

NickCommand::NickCommand(Client& client, const std::vector<std::string>& cmd)
    : CommandBase(client, cmd, false) {}

NickCommand::~NickCommand() {}

/**
 * @brief NICK <nickname>
 */
std::string NickCommand::execute() {
    Server *server = _client.getServer();
	std::string serverName = server->getName();

	if (_cmd.size() < 2) {
		return ERR_NEEDMOREPARAMS(_client.getPrefix(), "NICK");
	}

	std::string newNick = this->getParameter(1);

	int result = _client.setNick(newNick);
	if (result == 0) {
		return ERR_ERRONEUSNICKNAME(_client.getPrefix(), newNick);
	} else if (result == -1) {
		return ERR_NICKNAMEINUSE(newNick);
	}
	getClient().checkIdentification();
	return "";
}