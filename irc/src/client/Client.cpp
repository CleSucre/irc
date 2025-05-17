#include "Client.hpp"

Client::Client(Server *server, int fd) : _server(server), _fd(fd) {}

Client::~Client() {}

int Client::getFd() const {
    return _fd;
}

//==========================================

//==========================================


/**
 * @brief Listens for incoming messages from the client
 *        Read them on a temporary char[BUFFER_SIZE]
 * 		  If bytes > 0, stock them in client->buffer and search for a "\n" 
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

    _buff.append(buffer);
    size_t pos;
    while ((pos = _buff.find("\r\n")) != std::string::npos) {
        std::string arg = _buff.substr(0, pos);
        _buff.erase(0, pos + 2);
        packetRecieption(*this, arg);
    }
    return true;
}
