#include "CommandBase.hpp"

CommandBase::CommandBase(Client& client, const std::vector<std::string>& cmd)
    : _client(client), _cmd(cmd) {}

CommandBase::~CommandBase() {}

Client& CommandBase::getClient() const {
    return _client;
}

Server *CommandBase::getServer() const {
    return _client.getServer();
}

std::string CommandBase::getParameter(size_t index) const {
    if (index < _cmd.size()) {
        return _cmd[index];
    }
    return "";
}

std::string CommandBase::execute() {
    return "CommandBase execute method should not be called directly.";
}