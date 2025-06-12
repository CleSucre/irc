#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <iostream>
#include <unistd.h>
#include <cstring>
#include <vector>
#include <sstream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <openssl/ssl.h>
#include <openssl/err.h>
#include "colors.h"
#include "Server.hpp"


#define BUFFER_SIZE 1024
class Server ;

struct identification
{
    std::string Nickname;
    std::string Username;
    bool validPassword;
    bool certify;
};

class Client {
    private:
        Server *_server;
        std::string _buff;
        identification _id;
        int _fd;
        char* _ip;
        SSL* _ssl;
        bool _toDelete;

    public:
        Client(Server *server, int fd, char *ip, SSL* ssl);
        ~Client();

        int getFd() const;
        const char *getIp() const;
        int setNick(const std::string &);
        std::string getNick() const;
        bool setUser(const std::string &);
        std::string getUser() const;
        std::string getPrefix() const;
        void setValidPassword();

        Server* getServer() const;
        bool isSSL() const;
        bool listen();
        bool checkIdentification();

        void sendMessage(const std::string& message);

        bool shouldBeDeleted() const;
        void disconnect(const std::string& reason = "Client Quit");
};

void packetRecieption(Client& client, const std::string& packet);

#endif // CLIENT_HPP
