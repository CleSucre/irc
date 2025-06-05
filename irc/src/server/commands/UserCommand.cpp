#include "UserCommand.hpp"

UserCommand::UserCommand(Client& client, const std::vector<std::string>& cmd)
    : CommandBase(client, cmd, false) {}

UserCommand::~UserCommand() {
	
}

/**
 * @brief USER <username> <hostname> <servername> :<realname>
 */
std::string UserCommand::execute() {
	if (_client.setUser(getParameter(1))) {
		return ERR_NEEDMOREPARAMS(_client.getPrefix(), "USER");
	}
	return "001 RPL_WELCOME :Welcome to the Internet Relay Network " + _client.getNick();
}