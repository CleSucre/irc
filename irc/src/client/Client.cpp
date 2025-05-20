#include "Client.hpp"

Client::Client(Server *server, int fd, SSL* ssl) : _server(server), _fd(fd), _ssl(ssl) {}

Client::~Client() {
    if (_ssl) {
        SSL_shutdown(_ssl);
        SSL_free(_ssl);
    }
    close(_fd);
}

int Client::getFd() const {
    return _fd;
}

Server* Client::getServer() const {
    return _server;
}

bool Client::isSSL() const {
    return _ssl != NULL;
}

/**
 * @brief Listens for incoming messages from the client
 * 
 * @return bool true if the client is still connected, false otherwise
 */
bool Client::listen() {
    char buffer[BUFFER_SIZE];
    int bytes_read = 0;

    if (_ssl) {
        bytes_read = SSL_read(_ssl, buffer, sizeof(buffer) - 1);
    } else {
        bytes_read = recv(_fd, buffer, sizeof(buffer) - 1, 0);
    }

    if (bytes_read <= 0) {
        std::cerr << "Client disconnected or error" << std::endl;
        return false;
    }
    buffer[bytes_read] = '\0';
    std::cout << "Received: " << buffer << std::endl;
    return true;
}