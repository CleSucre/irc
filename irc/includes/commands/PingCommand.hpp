#ifndef PINGCOMMAND_HPP
#define PINGCOMMAND_HPP

#include "CommandBase.hpp"

class PingCommand : public CommandBase {
    public:
        PingCommand(Client& client, const std::vector<std::string>& cmd);
        virtual ~PingCommand();

        virtual void execute();
};

#endif // PINGCOMMAND_HPP
