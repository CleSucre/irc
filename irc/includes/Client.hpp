#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <iostream>
#include <unistd.h>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <openssl/ssl.h>
#include <openssl/err.h>
#include "colors.h"

#define BUFFER_SIZE 1024

class Server;

class Client {
    private:
        Server *_server;
        int _fd;
        SSL* _ssl;

    public:
        Client(Server *server, int fd, SSL* ssl);
        ~Client();

        int getFd() const;
        Server* getServer() const;
        bool isSSL() const;
        bool listen();
};

void packetRecieption(const Client& client, const std::string& packet);

#endif // CLIENT_HPP