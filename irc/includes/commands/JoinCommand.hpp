#ifndef JOINCOMMAND_HPP
#define JOINCOMMAND_HPP

#include "CommandBase.hpp"

class JoinCommand : public CommandBase {
    private:
        std::string generateJoinResponse(Client* client, Channel* channel);
        Channel* processChannel(const std::string& channelName, const std::string& key);

    public:
        JoinCommand(Client& client, const std::vector<std::string>& cmd);
        virtual ~JoinCommand();

        virtual void execute();
};

#endif // JOINCOMMAND_HPP