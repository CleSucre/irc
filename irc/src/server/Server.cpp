#include "Server.hpp"

Server::Server(std::string name, int port, const std::string& password, const std::string& certFile, const std::string& keyFile)
    : _name(name), _port(port), _password(password), _ssl_ctx(NULL), _server_fd(-1), _running(false) {

    if (!setupSSLContext(certFile.c_str(), keyFile.c_str())) {
        std::cerr << RED << "Failed to setup SSL context" << RESET << std::endl;
        _ssl_ctx = NULL;
    }
}

Server::~Server() {
    for (size_t i = 0; i < _clients.size(); ++i) {
        close(_clients[i]->getFd());
        delete _clients[i];
    }
    _clients.clear();

    if (_server_fd != -1) {
        close(_server_fd);
    }
}

/**
 * @brief Retrieves the server name
 *
 * @return const std::string& The server name
 */
const std::string& Server::getName() const {
    return _name;
}

/**
 * @brief Sets up the SSL context
 *
 * @param certFile Path to the certificate file
 * @param keyFile Path to the key file
 *
 * @return bool true on success, false on failure
 */
bool Server::setupSSLContext(const char* certFile, const char* keyFile) {
    if (access(certFile, F_OK) == -1) {
        std::cerr << RED << "Certificate file not found: " << certFile << RESET << std::endl;
        return false;
    }
    if (access(keyFile, F_OK) == -1) {
        std::cerr << RED << "Key file not found: " << keyFile << RESET << std::endl;
        return false;
    }

    SSL_library_init();
    OpenSSL_add_all_algorithms();
    SSL_load_error_strings();

    const SSL_METHOD* method = TLS_server_method();
    _ssl_ctx = SSL_CTX_new(method);
    if (!_ssl_ctx) {
        ERR_print_errors_fp(stderr);
        return false;
    }

    if (SSL_CTX_use_certificate_file(_ssl_ctx, certFile, SSL_FILETYPE_PEM) <= 0) {
        ERR_print_errors_fp(stderr);
        return false;
    }
    if (SSL_CTX_use_PrivateKey_file(_ssl_ctx, keyFile, SSL_FILETYPE_PEM) <= 0) {
        ERR_print_errors_fp(stderr);
        return false;
    }
    if (!SSL_CTX_check_private_key(_ssl_ctx)) {
        std::cerr << "Private key does not match the certificate public key" << std::endl;
        return false;
    }
    return true;
}

/**
 * @brief Initializes the server socket
 *
 * @return bool true on success, false on failure
 */
bool Server::setupSocket() {
    _server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (_server_fd == -1) {
        std::cerr << "Failed to create socket" << std::endl;
        return false;
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

/**
 * @brief Adds a new client to the server after a successful connection
 *
 * @param client The client to add
 *
 * @return bool true on success, false on failure
 */
bool Server::addClient(Client* client) {
    if (_clients.size() >= MAX_CLIENTS) {
        std::cerr << "Max clients reached" << std::endl;
        delete client;
        return false;
    }
    _clients.push_back(client);
    std::cout << GREEN << "Client " << YELLOW << client->getIp() << GREEN
              << " connected" << RESET << std::endl;
    return true;
}

/**
 * @brief Retrieves all clients on the server
 *
 * @return std::vector<Client*> A vector of pointers to all clients
 */
std::vector<Client*> Server::getAllClients() {
    return _clients;
}

/**
 * @brief Retrieves a client by its name
 *
 * @param name The name of the client to retrieve
 *
 * @return Client* Pointer to the client, or NULL if not found
 */
Client* Server::getClientByName(const std::string& name) {
    for (size_t i = 0; i < _clients.size(); ++i) {
        if (_clients[i]->getUser() == name) {
            return _clients[i];
        }
    }
    return NULL;
}

/**
 * @brief Adds a new channel to the server
 *
 * @param channel The channel to add
 *
 * @return bool true on success, false on failure
 */
bool Server::addChannel(Channel* channel) {
    _channels.push_back(channel);
    return true;
}

/**
 * @brief Removes a channel from the server
 *
 * @param channel The channel to remove
 *
 * @return bool true on success, false on failure
 */
bool Server::removeChannel(Channel* channel) {
    for (size_t i = 0; i < _channels.size(); ++i) {
        if (_channels[i] == channel) {
            delete _channels[i];
            _channels.erase(_channels.begin() + i);
            return true;
        }
    }
    return false;
}

/**
 * @brief Retrieves all channels on the server
 *
 * @return std::vector<Channel*> A vector of pointers to all channels
 */
std::vector<Channel*> Server::getAllChannels() {
    return _channels;
}

/**
 * @brief Retrieves a channel by its name
 *
 * @param name The name of the channel to retrieve
 *
 * @return Channel* Pointer to the channel, or NULL if not found
 */
Channel* Server::getChannelByName(const std::string& name) {
    for (size_t i = 0; i < _channels.size(); ++i) {
        if (_channels[i]->getName() == name) {
            return _channels[i];
        }
    }
    return NULL;
}

/**
 * @brief Resets the file descriptor set for select
 *
 * @param read_fds The file descriptor set to reset
 * @param max_fd The maximum file descriptor in the set
 */
void Server::resetReadFds(fd_set& read_fds, int& max_fd) {
    FD_ZERO(&read_fds);
    FD_SET(_server_fd, &read_fds);
    max_fd = _server_fd;

    for (size_t i = 0; i < _clients.size(); ++i) {
        int fd = _clients[i]->getFd();
        FD_SET(fd, &read_fds);
        if (fd > max_fd) {
            max_fd = fd;
        }
    }
}

/**
 * @brief Processes a new client connection
 *
 * @param _server_fd The server file descriptor to accept connections from
 *
 * @return bool true on success, false on failure
 */
bool Server::processNewClient(int _server_fd) {
    struct sockaddr_in client_addr;
    socklen_t client_len = sizeof(client_addr);

    int client_fd = accept(_server_fd, (struct sockaddr*)&client_addr, &client_len);
    if (client_fd == -1) {
        std::cerr << "Accept failed" << std::endl;
        return false;
    }

    SSL* ssl = NULL;
    if (_ssl_ctx != NULL) {
        ssl = SSL_new(_ssl_ctx);
        SSL_set_fd(ssl, client_fd);
        if (SSL_accept(ssl) <= 0) {
            SSL_free(ssl);
            ssl = NULL;
            std::cerr << "SSL handshake failed, accepting as plain connection" << std::endl;
        }
    }

    char* client_ip = inet_ntoa(client_addr.sin_addr);
    Client* client = new Client(this, client_fd, client_ip, ssl);
    if (!addClient(client)) {
        if (ssl) {
            SSL_shutdown(ssl);
            SSL_free(ssl);
        }
        delete client;
        close(client_fd);
        return false;
    }
    return true;
}

/**
 * @brief Processes the file descriptors for incoming connections and messages
 *
 * @param read_fds The file descriptor set to process
 * @param max_fd The maximum file descriptor in the set
 *
 * @return bool true on success, false on failure
 */
bool Server::processFds(fd_set read_fds, int max_fd) {
    if (select(max_fd + 1, &read_fds, NULL, NULL, NULL) == -1) {
        std::cerr << "select() failed" << std::endl;
        return false;
    }

    if (FD_ISSET(_server_fd, &read_fds)) {
        if (!processNewClient(_server_fd)) {
            std::cerr << RED << "Failed to process new client" << RESET << std::endl;
            return false;
        }
    }

    // Parcours inverse pour pouvoir supprimer les clients dans la boucle
    for (int i = (int)_clients.size() - 1; i >= 0; --i) {
        Client* client = _clients[i];
        if (FD_ISSET(client->getFd(), &read_fds)) {
            if (!client->listen()) {
                close(client->getFd());
                delete client;
                _clients.erase(_clients.begin() + i);
            }
        }
    }
    return true;
}

/**
 * @brief Checks if the provided password matches the server's password
 *
 * @param password The password to check
 *
 * @return bool true if the password matches, false otherwise
 */
bool Server::checkPassword(const std::string& password) const {
    return _password.empty() || _password == password;
}

/**
 * @brief Starts the IRC server and begins listening for clients
 */
void Server::start() {
    if (!setupSocket() || !bindSocket() || !listenSocket()) {
        std::cerr << RED << "Failed to setup/bind/listen socket" << RESET << std::endl;
        return;
    }

    _running = true;
    std::cout << GREEN << "Server started on port " << YELLOW
              << _port << GREEN << " with password " << YELLOW << _password << RESET << std::endl;

    fd_set read_fds;
    int max_fd;

    while (_running) {
        resetReadFds(read_fds, max_fd);

        if (!processFds(read_fds, max_fd)) {
            std::cerr << RED << "Error processing file descriptors" << RESET << std::endl;
            break;
        }
    }

    for (size_t i = 0; i < _clients.size(); ++i) {
        close(_clients[i]->getFd());
        delete _clients[i];
    }
    _clients.clear();

    close(_server_fd);
    std::cout << RED << "Server stopped" << RESET << std::endl;
}

/**
 * @brief Stops the IRC server
 */
void Server::stop() {
    _running = false;
}
