#include "Client.hpp"

Client::Client(Server *server, int fd) : _server(server), _fd(fd) {}

Client::~Client() {}

int Client::getFd() const {
    return _fd;
}

/**
 * @brief Listens for incoming messages from the client
 * 
 * @return bool true if the client is still connected, false otherwise
 */
bool Client::listen() {
    char buffer[BUFFER_SIZE];
    std::memset(buffer, 0, sizeof(buffer));
    int bytes_read = recv(_fd, buffer, sizeof(buffer) - 1, 0);
    if (bytes_read <= 0) {
        std::cerr << RED << "Client " << YELLOW << _fd << RED << " disconnected" << RESET << std::endl;
        return false;
    }
    buffer[bytes_read] = '\0';
    packetRecieption(*this, std::string(buffer));
    return true;
}