#ifndef SERVER_HPP
#define SERVER_HPP

#include <iostream>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <arpa/inet.h>

#include "Client.hpp"
#include "colors.h"

#define MAX_CLIENTS 10

class Server {
    private:
        int _port;
        std::string _password;
        int _server_fd;
        struct sockaddr_in _server_addr;
        bool _running;

        Client* _clients[MAX_CLIENTS];

        bool setupSocket();
        bool bindSocket();
        bool listenSocket();

        bool addClient(Client* client);
    public:
        Server(int port, const std::string& password);
        ~Server();

        void start();
        void stop();
};

#endif // SERVER_HPP