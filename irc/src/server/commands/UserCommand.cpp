#include "UserCommand.hpp"

UserCommand::UserCommand(Client& client, const std::vector<std::string>& cmd)
	: CommandBase(client, cmd, false) {}

UserCommand::~UserCommand() {

}

/**
 * @brief USER <username> <hostname> <servername> :<realname>
 */
std::string UserCommand::execute() {
	Server *server = _client.getServer();
	std::string serverName = server->getName();
	std::string username = getParameter(4);

	if (!_client.setUser(username)) {
		_client.sendMessage(":" + serverName + " " + ERR_NEEDMOREPARAMS(_client.getPrefix(), "USER") + "\r\n");
		return "";
	}
	getClient().checkIdentification();
	return "";
}
