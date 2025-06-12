#ifndef MODECOMMAND_HPP
#define MODECOMMAND_HPP

#include "CommandBase.hpp"

class ModeCommand : public CommandBase {
    public:
        ModeCommand(Client& client, const std::vector<std::string>& cmd);
        virtual ~ModeCommand();

        virtual void execute();
};

#endif // MODECOMMAND_HPP