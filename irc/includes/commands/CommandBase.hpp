#ifndef COMMANDBASE_HPP
#define COMMANDBASE_HPP

#include <sstream>
#include <string>
#include <vector>
#include "Server.hpp"
#include "Utils.hpp"
#include "Response.hpp"

class CommandBase {
    protected:
        Client& _client;
        std::vector<std::string> _cmd;
        bool _needAuth;

        Client& getClient() const;
        Server *getServer() const;
        bool needAuth() const;

        std::string getParameter(size_t index) const;

        virtual void execute() = 0;
    public:
        CommandBase(Client& client, const std::vector<std::string>& cmd, bool needAuth);
        virtual ~CommandBase();

        void pre_execute();
};

#endif // COMMANDBASE_HPP
