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
    if (index >= _cmd.size())
        return "";

    std::string param = _cmd[index];

    if (!param.empty() && param[0] == ':') {
        for (size_t i = index + 1; i < _cmd.size(); ++i) {
            param += " " + _cmd[i];
        }
        return substr(param, 1, param.size() - 1);
    }

    return param;
}

void CommandBase::pre_execute() {
    if (_needAuth && !_client.checkIdentification()) {
		_client.sendMessage(":" + getServer()->getName() + " " + ERR_NOTREGISTERED(_client.getPrefix()) + "\r\n");
        return;
    } else if (_cmd.empty()) {
		_client.sendMessage(":" + getServer()->getName() + " " + ERR_UNKNOWNCOMMAND(_client.getPrefix(), "") + "\r\n");
        return;
    }
    return execute();
}

void CommandBase::execute() {}
