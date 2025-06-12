#ifndef KICKCOMMAND_HPP
#define KICKCOMMAND_HPP

#include "CommandBase.hpp"

class KickCommand : public CommandBase {
    public:
        KickCommand(Client& client, const std::vector<std::string>& cmd);
        virtual ~KickCommand();

        virtual void execute();
};

#endif // KICKCOMMAND_HPP