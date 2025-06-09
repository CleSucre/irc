#ifndef QUITCOMMAND_HPP
#define QUITCOMMAND_HPP

#include "CommandBase.hpp"

class QuitCommand : public CommandBase {
    public:
        QuitCommand(Client& client, const std::vector<std::string>& cmd);
        virtual ~QuitCommand();

        virtual std::string execute();
};

#endif // QUITCOMMAND_HPP