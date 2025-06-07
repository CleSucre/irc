#include "UserCommand.hpp"

UserCommand::UserCommand(Client& client, const std::vector<std::string>& cmd)
	: CommandBase(client, cmd, false) {}

UserCommand::~UserCommand() {}

/**
 * @brief USER <username> <hostname> <servername> :<realname>
 */
std::string UserCommand::execute() {
	return "";
}
