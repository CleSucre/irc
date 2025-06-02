#ifndef INVITECOMMAND_HPP
#define INVITECOMMAND_HPP

#include "CommandBase.hpp"

class InviteCommand : public CommandBase {
    public:
        InviteCommand(Client& client, const std::vector<std::string>& cmd);
        virtual ~InviteCommand();

        virtual std::string execute();
};

#endif // INVITECOMMAND_HPP