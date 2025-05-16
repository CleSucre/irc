#include "Server.hpp"

Server::Server(int port, const std::string& password) : _port(port), _password(password) {
    for (int i = 0; i < MAX_CLIENTS; ++i) {
        _clients[i] = NULL;
    }
    _server_fd = -1;
    _running = false;
}

Server::~Server() {}

/**
 * @brief Initializes the server socket
 * 
 * @return bool true on success, false on failure
 */
bool Server::setupSocket() {
    _server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (_server_fd == -1) {
        std::cerr << "Failed to create socket" << std::endl;
        return 0;
    }

    int opt = 1;
    if (setsockopt(_server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1) {
        std::cerr << "setsockopt failed" << std::endl;
        close(_server_fd);
        return false;
    }
    return true;
}

/**
 * @brief Binds the server socket to the specified port
 * 
 * @return bool true on success, false on failure
 */
bool Server::bindSocket() {
    _server_addr.sin_family = AF_INET;
    _server_addr.sin_addr.s_addr = INADDR_ANY;
    _server_addr.sin_port = htons(_port);

    if (bind(_server_fd, (struct sockaddr *)&_server_addr, sizeof(_server_addr)) == -1) {
        std::cerr << "Bind failed" << std::endl;
        close(_server_fd);
        return false;
    }
    return true;
}

/**
 * @brief Listens for incoming connections on the server socket
 * 
 * @return bool true on success, false on failure
 */
bool Server::listenSocket() {
    if (listen(_server_fd, MAX_CLIENTS) == -1) {
        std::cerr << "Listen failed" << std::endl;
        close(_server_fd);
        return false;
    }
    return true;
}

bool Server::addClient(Client* client) {
    for (int i = 0; i < MAX_CLIENTS; ++i) {
        if (_clients[i] == NULL) {
            _clients[i] = client;
            std::cout << GREEN << "Client " << YELLOW << client->getFd() << GREEN
                    << " connected" << RESET << std::endl;
            return true;
        }
    }
    std::cerr << "Max clients reached" << std::endl;
    delete client;
    return false;
}

/**
 * @brief Starts the IRC server and begins listening for clients
 */
void Server::start() {
    if (!setupSocket()) {
        std::cerr << RED << "Failed to setup socket" << RESET << std::endl;
        return;
    }
    if (!bindSocket()) {
        std::cerr << RED << "Failed to bind socket" << RESET << std::endl;
        return;
    }
    if (!listenSocket()) {
        std::cerr << RED << "Failed to listen on socket" << RESET << std::endl;
        return;
    }

    _running = true;
    std::cout << GREEN << "Server started on port " << YELLOW
            << _port << GREEN << " with password " << YELLOW << _password << RESET << std::endl;

    fd_set read_fds;
    int max_fd;

    while (_running) {
        FD_ZERO(&read_fds);
        FD_SET(_server_fd, &read_fds);
        max_fd = _server_fd;

        for (int i = 0; i < MAX_CLIENTS; ++i) {
            if (_clients[i]) {
                int fd = _clients[i]->getFd();
                FD_SET(fd, &read_fds);
                if (fd > max_fd) {
                    max_fd = fd;
                }
            }
        }

        if (select(max_fd + 1, &read_fds, NULL, NULL, NULL) == -1) {
            std::cerr << "select() failed" << std::endl;
            break;
        }

        if (FD_ISSET(_server_fd, &read_fds)) {
            int client_fd = accept(_server_fd, NULL, NULL);
            if (client_fd != -1) {
                Client* client = new Client(this, client_fd);
                if (!addClient(client)) {
                    close(client_fd);
                }
            }
        }

        for (int i = 0; i < MAX_CLIENTS; ++i) {
            if (_clients[i] && FD_ISSET(_clients[i]->getFd(), &read_fds)) {
                if (!_clients[i]->listen()) {
                    close(_clients[i]->getFd());
                    delete _clients[i];
                    _clients[i] = NULL;
                }
            }
        }
    }

    for (int i = 0; i < MAX_CLIENTS; ++i) {
        if (_clients[i]) {
            close(_clients[i]->getFd());
            delete _clients[i];
            _clients[i] = NULL;
        }
    }

    close(_server_fd);
    std::cout << RED << "Server stopped" << RESET << std::endl;
}

/**
 * @brief Stops the IRC server
 */
void Server::stop() {
    _running = false;
}