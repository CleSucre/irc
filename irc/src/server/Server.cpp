#include "Server.hpp"

Server::Server(int port, const std::string& password) : port(port), password(password) {}

Server::Server(const Server& other) : port(other.port), password(other.password) {}

Server& Server::operator=(const Server& other) {
    if (this != &other) {
        port = other.port;
        password = other.password;
    }
    return *this;
}

Server::~Server() {}

void Server::start() {
    std::cout << GREEN << "Server started on port " << port << " with password " << password << RESET << std::endl;
    //TODO: start irc server HERE

    
}
void Server::stop() {
    //TODO: stop irc server HERE
    std::cout << RED << "Server stopped" << RESET << std::endl;
}