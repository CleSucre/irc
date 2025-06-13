#ifndef LISTCOMMAND_HPP
#define LISTCOMMAND_HPP

#include "CommandBase.hpp"

class ListCommand : public CommandBase {
    public:
        ListCommand(Client& client, const std::vector<std::string>& cmd);
        virtual ~ListCommand();

        virtual void execute();
};

#endif // LISTCOMMAND_HPP
