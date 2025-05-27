#ifndef TOPICCOMMAND_HPP
#define TOPICCOMMAND_HPP

#include "CommandBase.hpp"

class TopicCommand : public CommandBase {
    public:
        TopicCommand(Client& client, const std::vector<std::string>& cmd);
        virtual ~TopicCommand();

        virtual std::string execute();
};

#endif // TOPICCOMMAND_HPP