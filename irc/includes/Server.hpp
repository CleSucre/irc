#ifndef SERVER_HPP
#define SERVER_HPP

#include <iostream>
#include <sstream>
#include <string>
#include "colors.h"

class Server {
    private:
        int port;
        std::string password;

    public:
        Server(int port, const std::string& password);
        Server(const Server& other);
        Server& operator=(const Server& other);
        ~Server();

        void start();
        void stop();
};

#endif // SERVER_HPP