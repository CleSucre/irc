#ifndef COMMANDBASE_HPP
#define COMMANDBASE_HPP

#include <sstream>
#include <string>
#include <vector>
#include "Server.hpp"
#include "Utils.hpp"

class CommandBase {
    protected:
        Client& _client;
        std::vector<std::string> _cmd;

        Client& getClient() const;
        Server *getServer() const;

        std::string getParameter(size_t index) const;

    public:
        CommandBase(Client& client, const std::vector<std::string>& cmd);
        virtual ~CommandBase();

        virtual std::string execute() = 0;
};

#endif // COMMANDBASE_HPP