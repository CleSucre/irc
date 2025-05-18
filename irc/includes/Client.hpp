#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <iostream>
#include <unistd.h>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include "colors.h"

#define BUFFER_SIZE 1024

class Server;

struct identification
{
    std::string Nickname;
    std::string Username;
    bool certify;
};

class Client {
    private:
        Server *_server;
        std::string _buff;
        identification _id;
        int _fd;

    public:
        Client(Server *server, int fd);
        ~Client();

        int getFd() const;
        void setNick(const std::string &);
        bool setUser(const std::string &);
        bool listen();
        bool checkIdentification();
        bool go_command(std::string arg);
};

void packetRecieption(const Client& client, const std::string& packet);

#endif // CLIENT_HPP