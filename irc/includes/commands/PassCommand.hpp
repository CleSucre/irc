#ifndef PASSCOMMAND_HPP
#define PASSCOMMAND_HPP

#include "CommandBase.hpp"

class PassCommand : public CommandBase {
    public:
        PassCommand(Client& client, const std::vector<std::string>& cmd);
        virtual ~PassCommand();

        virtual void execute();
};

#endif // PASSCOMMAND_HPP