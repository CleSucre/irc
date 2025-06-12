#ifndef NICKCOMMAND_HPP
#define NICKCOMMAND_HPP

#include "CommandBase.hpp"

class NickCommand : public CommandBase {
    public:
        NickCommand(Client& client, const std::vector<std::string>& cmd);
        virtual ~NickCommand();

        virtual void execute();
};

#endif // NICKCOMMAND_HPP