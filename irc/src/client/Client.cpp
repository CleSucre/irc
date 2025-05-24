#include "Client.hpp"
#include <sstream>
#include <vector>

/**
 * 
001–099 : messages de statut et d’information pour l’enregistrement
200–399 : réponses de commandes réussies (par ex. LIST, WHOIS…)
400–599 : erreurs (ERR_*)
600–999 : messages de statut et d’information pour les commandes de gestion de réseau
Que faire a la connexion :
PASS               # (optionnel, si le serveur n'exige pas de mot de passe, vous pouvez sauter cette ligne)
NICK MonPseudo
USER MonIdent 0 * :Mon Nom Réel
 */

Client::Client(Server *server, int fd, char *ip, SSL* ssl) : _server(server), _fd(fd), _ip(ip), _ssl(ssl) {
   _id.certify = false;
}

Client::~Client() {
    if (_ssl) {
        SSL_shutdown(_ssl);
        SSL_free(_ssl);
    }
    close(_fd);
}

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
 * @brief Set the NickName of the client
 * 		  Do nothing if new NickName is empty
 * @param name : NickName to set
 * @return void
 */
void Client::setNick(const std::string &name)
{
    if (name.length() < 1)
		return ;
	else if (name.length() > 9 || name.find(" ") != std::string::npos)
	{
		std::cerr << "Client " << _id.Username << " has entered a wrong NickName." << std::endl;
		return ;
	}
    //TODO: Check if new NickName isn't already taken in the server and in channels
	if (_id.Nickname != name)
		_id.Nickname = name;
	std::cout << "Client " << _id.Username << " has entered a new NickName : " << _id.Nickname << std::endl;
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

 * @brief Parse the username arguments from USER command
 * 			username : 0-9 , (-), etc., no space or controle characters,  < 9 characters
			No # or ',' '.'
 * @param args : arguments to parse
 * @param username : pointer to the username to fill
 * @return true if ok, false if error
 */
static bool isValidUsername(const std::string& username) {
	size_t len = username.length();
    if (username.empty() || len >= 9)
        return false;
    for (std::string::size_type i = 0; i < len; ++i) {
        char c = username[i];
        if (c < 32 || c == ' ' || c == ':' || c == '#' || c == ',' || (!std::isalnum(c) && c != '-'))
            return false;
    }
    return true;
}

/**
 * @brief Parse the arguments of the USER command
 * 		Fill the username with everything between 3thrd 
 * 		count and last one without first occurence en ':'
 * @param args : arguments to parse
 * @param username : pointer to the username to fill
 * @return true if ok, false if error
 */
#
static bool parsecolon(const std::string &args, std::string *username)
{
	std::string::size_type limit;
	limit = args.find(":");
	if (limit == std::string::npos)
	{
		std::cerr << "ERR_NEEDMOREPARAMS :Not enough parameters" << std::endl;
		return (false);
	}
	std::string check = args.substr(0, limit);
	std::istringstream iss(check);
	std::string param;
	std::vector<std::string> tokens;
	int count;
	for (count = 0; iss >> param; count++)
	{
		if (count == 0 && isValidUsername(param) == false)
		{
			std::cerr << "ERR_INVALIDPARAMS :Invalid username" << std::endl;
			return (false);
		}
		tokens.push_back(param);
	}
	if (count < 3)
	{
		std::cerr << "ERR_NEEDMOREPARAMS :Not enough parameters" << std::endl;
		return (false);
	}
	for(size_t i = 3; i < tokens.size(); i++)
		(*username).append(tokens[i] + " ");
	(*username).append(args.substr(limit + 1));
	return (true);
}


/**
 * @brief Set the UserName of the client
 * @throw ERR_NEEDMOREPARAMS : Not enough parameters
 * @throw ERR_ALREADYREGISTERED : Already registered
 * @example : USER guest tolmoon tolsun :Ronnie Reagan
 * 			USER <username> <hostname> <servername> :<realname>
 * @param name : UserName to set
 * @return true if ok, false if error
 */
bool Client::setUser(const std::string &args)
{
	/**
		hostname et servername : format d’hôte (lettres, chiffres, points). <= 255 caractères 
		realname : peut contenir espaces, mais pas \r/\n.
	 */
	//Check if the client is already registered
	if (_id.certify == true || _id.Username.length() > 0)
	{
		std::cerr << "462 ERR_ALREADYREGISTERED :Unauthorized command (already registered) "<< std::endl;
		return (false);
	}
	std::string username;
	parsecolon(args, &username);
	if (username.length() < 1 || username.length() > 75)
	{
		std::cerr << "Client " << _id.Username << " has entered a wrong UserName." << std::endl;
		return (false);
	}
	//TODO: Check if new UserName isn't already taken in the server and in channels --- Same function as in Nickname
	_id.Username = username;
	std::cout << "Client " << _fd << " is now known as : " << _id.Username << std::endl;
    return (true);
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


//==========================================

//==========================================

/*
PASS après enregistrement
Selon RFC 2812, la commande PASS « MUST be sent before any attempt to register the connection is made » ; si elle arrive après l’enregistrement, le serveur renvoie
*/

/**
 * @brief Check if the client is identified and set Nickname and Username if needed
 * 
 * @return false if error, true if ok
 */
bool Client::checkIdentification()
{
	if (_id.certify == false && _buff.find("NICK") == std::string::npos\
		&& _buff.find("USER") == std::string::npos)
	{
		std::cerr << "ERR_NOTREGISTERED :You have not registered" << std::endl; // TODO: Send this error to the client
		return false;
	}
	if (_buff.find("NICK") != std::string::npos)
	{
		setNick(_buff.substr(_buff.find("NICK") + 5));
		_buff.erase(0, _buff.find("\r\n") + 2);
	}
	else if (_buff.find("USER") != std::string::npos)
	{
		setUser(_buff.substr(_buff.find("USER") + 5));
		_buff.erase(0, _buff.find("\r\n") + 2);
	}
	if (_id.Username.length() > 0  && _id.Nickname.length() > 0)
		_id.certify = true;
	return true;
}
/**
 * @brief Check if the command is valid
 * @param arg : input command to check
 * 
 * @return true if command is valid
 * @return false if error
 */
bool Client::go_command(std::string arg)
{
	std::string command[4] = {"KICK", "INVITE", "TOPIC", "MODE"};
	for (int i = 0; i < 4; i++)
	{
		if (arg.find(command[i]) != std::string::npos)
		{
			std::cout << "Command found : " << command[i] << std::endl;
			return true;
		}
	}
	std::cout << "Command not found" << std::endl;
	_buff.erase(0, _buff.size());
	std::cout << "Error in input : " << arg << std::endl;
    return false;
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
        std::cerr << "Client disconnected or error" << std::endl;
        return false;
    }
	buffer[bytes_read] = '\0';
	// Concatenate the buffer to the client's buffer
	_buff.append(buffer);
	// Check identification
	if (checkIdentification() == false)
    {
        _buff.erase(0, _buff.size());
		return true;
    }
	// Check if the buffer contains a complete command
    size_t pos;
    while ((pos = _buff.find("\r\n")) != std::string::npos) {
        std::string arg = _buff.substr(0, pos);
        _buff.erase(0, pos + 2);
		// TODO: Parse command and send it to person C + change this function name
        go_command(arg);
		// Valid packet received
        packetRecieption(*this, arg);
        
    }
	std::cout << "END :: Here is buff : [" << _buff << "]" << std::endl;
    return true;
}

/**
 * @brief Send a message to the client
 * 
 * @param message : message to send
 * @return true if ok, false if error
 */
void Client::sendMessage(const std::string& message) {
	if (_ssl) {
		SSL_write(_ssl, message.c_str(), message.length());
	} else {
		send(_fd, message.c_str(), message.length(), 0);
	}
}