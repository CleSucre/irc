#include "UserCommand.hpp"

UserCommand::UserCommand(Client& client, const std::vector<std::string>& cmd)
	: CommandBase(client, cmd, false) {}

UserCommand::~UserCommand() {

}

/**
 * @brief USER <username> <hostname> <servername> :<realname>
 */

void UserCommand::execute() {
	if (_cmd.size() < 5) {
		_client.sendMessage(":" + getServer()->getName() + " " + ERR_NEEDMOREPARAMS(_client.getPrefix(), "USER"));
		return;
	}
	std::string username = getParameter(1);

	if (!_client.setUser(username)) {
		return;
	}
	getClient().checkIdentification();
}
