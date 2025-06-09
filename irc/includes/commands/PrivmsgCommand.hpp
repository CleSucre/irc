#ifndef PRIVMSGCOMMAND_HPP
#define PRIVMSGCOMMAND_HPP

#include "CommandBase.hpp"

class PrivmsgCommand : public CommandBase {
    public:
        PrivmsgCommand(Client& client, const std::vector<std::string>& cmd);
        virtual ~PrivmsgCommand();

        virtual std::string execute();
};

#endif // PRIVMSGCOMMAND_HPP