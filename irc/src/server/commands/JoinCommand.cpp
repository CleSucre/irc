#include "JoinCommand.hpp"

JoinCommand::JoinCommand(Client& client, const std::vector<std::string>& cmd)
    : CommandBase(client, cmd) {}

JoinCommand::~JoinCommand() {}

std::string JoinCommand::execute() {
    this->getClient().sendMessage("JOIN command received");
    return "JOIN command executed";
}