#ifndef PARTCOMMAND_HPP
#define PARTCOMMAND_HPP

#include "CommandBase.hpp"

class PartCommand : public CommandBase {
    public:
        PartCommand(Client& client, const std::vector<std::string>& cmd);
        virtual ~PartCommand();

        virtual void execute();
};

#endif // PARTCOMMAND_HPP