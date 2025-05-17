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

class Client {
    private:
        Server *_server;
        std::string _buff;
        int _fd;

    public:
        Client(Server *server, int fd);
        ~Client();

        int getFd() const;
        bool listen();
};

void packetRecieption(const Client& client, const std::string& packet);

#endif // CLIENT_HPP