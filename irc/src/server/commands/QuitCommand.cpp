#include "QuitCommand.hpp"

QuitCommand::QuitCommand(Client& client, const std::vector<std::string>& cmd)
    : CommandBase(client, cmd, true) {}

QuitCommand::~QuitCommand() {}

/**
 * @brief QUIT [ :<Quit message> ]
 */
void QuitCommand::execute() {
	std::string message;
	if (_cmd.size() > 1) {
		message = getParameter(1);
	} else {
		message = "Client Quit";
	}
	_client.disconnect(message);
}