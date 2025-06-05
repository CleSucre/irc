#include "NickCommand.hpp"

NickCommand::NickCommand(Client& client, const std::vector<std::string>& cmd)
    : CommandBase(client, cmd, false) {}

NickCommand::~NickCommand() {}

/**
 * @brief NICK <nickname>
 */
std::string NickCommand::execute() {
	std::cout << "Executing NICK command for client: " << _client.getNick() << std::endl;
    Server *server = _client.getServer();
	std::string serverName = server->getName();

	if (_cmd.size() < 2) {
		return ERR_NEEDMOREPARAMS(_client.getPrefix(), "NICK");
	}

	std::string newNick = this->getParameter(1);

	if (!_client.setNick(newNick)) {
		return ERR_ERRONEUSNICKNAME(_client.getPrefix(), newNick);
	}
	return "";
}