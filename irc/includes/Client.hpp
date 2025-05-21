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
        SSL* _ssl;

    public:
        Client(Server *server, int fd, SSL* ssl);
        ~Client();

        int getFd() const;
        void setNick(const std::string &);
        std::string getNick() const;
        bool setUser(const std::string &);
        std::string getUser() const;

        Server* getServer() const;
        bool isSSL() const;
        bool listen();
        bool checkIdentification();
        bool go_command(std::string arg);

        void sendMessage(const std::string& message);
};

void packetRecieption(Client& client, const std::string& packet);

#endif // CLIENT_HPP