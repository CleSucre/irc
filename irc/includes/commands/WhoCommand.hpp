#ifndef WHOCOMMAND_HPP
#define WHOCOMMAND_HPP

#include "CommandBase.hpp"

class WhoCommand : public CommandBase {
    public:
        WhoCommand(Client& client, const std::vector<std::string>& cmd);
        virtual ~WhoCommand();

        virtual void execute();
};

#endif // WHOCOMMAND_HPP
