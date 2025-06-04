#include "CommandBase.hpp"

CommandBase::CommandBase(Client& client, const std::vector<std::string>& cmd, bool needAuth = true)
    : _client(client), _cmd(cmd), _needAuth(needAuth) {}

CommandBase::~CommandBase() {}

Client& CommandBase::getClient() const {
    return _client;
}

Server *CommandBase::getServer() const {
    return _client.getServer();
}

bool CommandBase::needAuth() const {
    return _needAuth;
}

std::string CommandBase::getParameter(size_t index) const {
    if (index < _cmd.size()) {
        return _cmd[index];
    }
    return "";
}

std::string CommandBase::pre_execute() {
    if (_needAuth && !_client.checkIdentification()) {
        return ERR_NOTREGISTERED(_client.getPrefix());
    }
    if (_cmd.empty()) {
        return ERR_UNKNOWNCOMMAND(_client.getPrefix(), "");
    }
    return execute();
}

std::string CommandBase::execute() {
    return "";
}