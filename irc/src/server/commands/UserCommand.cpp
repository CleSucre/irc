#include "UserCommand.hpp"

UserCommand::UserCommand(Client& client, const std::vector<std::string>& cmd)
	: CommandBase(client, cmd, false) {}

UserCommand::~UserCommand() {

}

/**
 * @brief USER <username> <hostname> <servername> :<realname>
 */
void UserCommand::execute() {
	std::string username = getParameter(4);

	if (!_client.setUser(username)) {
		_client.sendMessage(":" + getServer()->getName() + " " + ERR_NEEDMOREPARAMS(_client.getPrefix(), "USER"));
		return;
	}
	getClient().checkIdentification();
}
