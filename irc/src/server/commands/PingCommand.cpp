#include "PingCommand.hpp"

PingCommand::PingCommand(Client& client, const std::vector<std::string>& cmd)
	: CommandBase(client, cmd, true) {}

PingCommand::~PingCommand() {}

/**
 * @brief PING [msg]
 */
std::string PingCommand::execute() {
	Server* server = _client.getServer();
	std::string serverName = server->getName();

	if (_cmd.size() < 2) {
		_client.sendMessage(":" + serverName + " 409 " + _client.getNick() + " :No origin specified\r\n");
		return "";
	}

	_client.sendMessage(":" + serverName + " PONG " + serverName + " :" + joinFirstN(_cmd, 2) + "\r\n");
	return "";
}
