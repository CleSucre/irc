#include "Client.hpp"
#include "Utils.hpp"

/* ============================================= \\
//					Constructors			 	 \\
// =============================================*/

Client::Client(Server *server, int fd, char *ip, SSL* ssl) : _server(server), _fd(fd), _ip(ip), _ssl(ssl) {
   _id.validPassword = false;
   _id.certify = false;
   _toDelete = false;
}

Client::~Client() {
	if (_ssl) {
		SSL_shutdown(_ssl);
		SSL_free(_ssl);
	}
	close(_fd);
}

/*=============================================\\
//					Getters						\\
//=============================================	*/

/**
 * @brief Get the file descriptor of the client
 *
 * @return int : file descriptor
 */
int Client::getFd() const {
	return _fd;
}

const char *Client::getIp() const {
	return _ip;
}

Server* Client::getServer() const {
	return _server;
}

bool Client::isSSL() const {
	return _ssl != NULL;
}

/**
 * @brief Get the NickName of the client
 *
 * @return std::string : NickName
 */
std::string Client::getNick() const
{
	return _id.Nickname;
}

/**
 * @brief Get the UserName of the client
 *
 * @return std::string : UserName
 */
std::string Client::getUser() const
{
	return _id.Username;
}

/**
 * @brief Get the prefix of the client
 *
 * @return std::string : prefix
 */
std::string Client::getPrefix() const {
	std::string prefix;
	if (_id.certify == true) {
		prefix = ":" + _id.Nickname + "!" + _id.Username + "@" + std::string(_ip);
	} else {
		prefix = ":" + std::string(_ip);
	}
	return prefix;
}

/**
 * @brief Set valid password
 *
 * This function sets the validPassword attribute of the client to true.
 */
void Client::setValidPassword() {
	_id.validPassword = true;
}

/**
 * @brief Listens for incoming messages from the client
 *        Read them on a temporary char[BUFFER_SIZE]
 * 		  If bytes > 0, stock them in client->buffer and search for a "\n"
 *
 * @return true if ok, false if error
 */
bool Client::listen() {
	// Listen for incoming messages from the client inside a buffer
	char buffer[BUFFER_SIZE];
	int bytes_read = 0;

	if (_ssl) {
		bytes_read = SSL_read(_ssl, buffer, sizeof(buffer) - 1);
	} else {
		bytes_read = recv(_fd, buffer, sizeof(buffer) - 1, 0);
	}

	if (bytes_read <= 0) {
		disconnect();
		return false;
	}
	buffer[bytes_read] = '\0';
	// Concatenate the buffer to the client's buffer
	_buff.append(buffer);

	// Check if the buffer contains a complete command
	size_t pos;
	while ((pos = _buff.find("\r\n")) != std::string::npos) {
		std::string arg = _buff.substr(0, pos);
		_buff.erase(0, pos + 2);
		// Valid packet received
		packetRecieption(*this, arg);

	}
	return true;
}

/**
 * @brief Send a message to the client
 *
 * @param message : message to send
 * @return true if ok, false if error
 */
void Client::sendMessage(const std::string& message) {
	std::string formattedMessage = message + "\r\n";
	if (_ssl) {
		SSL_write(_ssl, formattedMessage.c_str(), formattedMessage.length());
	} else {
		send(_fd, formattedMessage.c_str(), formattedMessage.length(), 0);
	}
	std::cout << "Packet sent to client " << getPrefix() << ": " << formattedMessage << std::endl;
}

/**
 * @brief Check if the client should be deleted
 * 
 * This function checks if the client is marked for deletion.
 * @return true if the client should be deleted, false otherwise
 */
bool Client::shouldBeDeleted() const {
    return _toDelete;
}

/**
 * @brief Disconnect the client properly in any case
 * 
 * This function will shutdown the SSL connection if it exists,
 * free the SSL structure, close the file descriptor,
 * and remove the client from the server's client list.
 * 
 * @param reason : reason for disconnection, default is "Client Quit"
 */
void Client::disconnect(const std::string& reason) {
    std::cout << "Disconnecting client " << _ip << ": " << reason << std::endl;
    _toDelete = true;
}