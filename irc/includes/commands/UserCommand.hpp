#ifndef USERCOMMAND_HPP
#define USERCOMMAND_HPP

#include "CommandBase.hpp"

class UserCommand : public CommandBase {
    public:
        UserCommand(Client& client, const std::vector<std::string>& cmd);
        virtual ~UserCommand();

        virtual void execute();
};

#endif // USERCOMMAND_HPP