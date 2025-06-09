#ifndef JOINCOMMAND_HPP
#define JOINCOMMAND_HPP

#include "CommandBase.hpp"

class JoinCommand : public CommandBase {
    public:
        JoinCommand(Client& client, const std::vector<std::string>& cmd);
        virtual ~JoinCommand();

        virtual void execute();
};

#endif // JOINCOMMAND_HPP