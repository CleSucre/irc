#ifndef SERVER_HPP
#define SERVER_HPP

#include <iostream>
#include <string.h>
#include <vector>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <openssl/ssl.h>
#include <openssl/err.h>
#include <csignal>
#include "Client.hpp"
#include "colors.h"

#define MAX_CLIENTS 10

class Channel;
class Client;
#include "Channel.hpp"

class Server {
    private:
        std::string _name;
        int _port;
        std::string _password;
        SSL_CTX* _ssl_ctx;
        int _server_fd;
        struct sockaddr_in _server_addr;
        bool _running;

        std::vector<Client*> _clients;
        std::vector<Channel*> _channels;

        bool setupSSLContext(const char* certFile, const char* keyFile);
        bool setupSocket();
        bool bindSocket();
        bool listenSocket();

        void resetReadFds(fd_set& read_fds, int& max_fd);
        bool processNewClient(int client_fd);
        bool processFds(fd_set read_fds, int max_fd);

        void removeClientInChannel(Client *client);

    public:
        Server(std::string name, int port, const std::string& password, const std::string& certFile, const std::string& keyFile);
        ~Server();

        void start();
        void stop();

        const std::string& getName() const;

        bool checkPassword(const std::string& password) const;

        bool addClient(Client* client);
        std::vector<Client*> getAllClients();
        Client *getClientByName(const std::string& name);
        Client *getClientByNickname(const std::string& name);

        bool addChannel(Channel* channel);
        bool removeChannel(Channel* channel);
        std::vector<Channel*> getAllChannels();
        Channel *getChannelByName(const std::string& name);
};

#endif // SERVER_HPP
