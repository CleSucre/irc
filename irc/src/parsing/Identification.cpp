#include "Client.hpp"
#include "Utils.hpp"
#include "Error.hpp"

/**
 * @brief Set the NickName of the client
 * 		  Do nothing if new NickName is empty
 * @param name : NickName to set
 * @return 1 if ok, 0 if error or -1 if already registered
 */
int Client::setNick(const std::string &name)
{
    if (name.length() < 1)
	{
		std::cerr << "Client " << _ip << " has entered an empty NickName." << std::endl;
		return 0;
	}
	else if (name.length() > 10 || name.find(" ") != std::string::npos)
	{
		std::cerr << "Client " << _ip << " has entered a wrong NickName : " << name << std::endl;
		return 0;
	}
	if (!std::isalpha(name[0]))
        return false;
    for (size_t i = 0; i < name.length(); ++i) {
        char c = name[i];
        if (!std::isalnum(c) && c != '-' && c != '[' && c != ']' && c != '\\') {
            return false;
        }
    }
    //TODO: Check if new NickName isn't already taken in the server and in channels
	if (_server->getClientByNickname(name) != NULL)
	{
		std::cerr<< "Client " << _ip << " has entered a NickName already taken : " << name << std::endl;
		return -1;
	}
	if (_id.Nickname != name)
		_id.Nickname = name;
	std::cout << "Client " << _ip << " has entered a new NickName : " << _id.Nickname << std::endl;
	return 1;
}

/**
 * @brief Parse the username arguments from USER command
 * 			username : 0-9 , (-), etc., no space or controle characters,  < 9 characters
			No # or ',' '.'
 * @param args : arguments to parse
 * @param username : pointer to the username to fill
 * @return true if ok, false if error
 */
static bool isValidUsername(const std::string& username) {
	size_t len = username.length();
    if (username.empty() || len < 1 || len > 75)
        return false;
    for (std::string::size_type i = 0; i < len; ++i) {
        char c = username[i];
        if (c < 32 || c == ' ' || c == ':' || c == '#' || c == ',' || (!std::isalnum(c) && c != '-'))
            return false;
    }
    return true;
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
bool Client::setUser(const std::string &username)
{
	/**
		hostname et servername : format d’hôte (lettres, chiffres, points). <= 255 caractères
		realname : peut contenir espaces, mais pas \r/\n.
	 */
	if (_id.certify == true || !_id.Username.empty()) {
		sendMessage(_server->getName() + " " + ERR_ALREADYREGISTRED(_id.Username));
		std::cerr << "Client " << _id.Username << " is already registered." << std::endl;
		return (false);
	}
	if (!isValidUsername(username)) {
		sendMessage(_server->getName() + " " + ERR_ERRONEUSNICKNAME(_id.Username, username));
		std::cerr << "Client " << _id.Username << " has entered a wrong UserName." << std::endl;
		return (false);
	}
	if (_server->getClientByName(username) != NULL)
	{
		std::cerr<< "Client " << _fd << " has entered a NickName already taken : " << username << std::endl;
		sendMessage(_server->getName() + " " + ERR_NICKNAMEINUSE(username));
		return false;
	}
	//TODO: Check if new UserName isn't already taken in the server and in channels --- Same function as in Nickname
	_id.Username = username;
	std::cout << "Client " << _ip << " is now known as : " << _id.Username << std::endl;
    return (true);
}

/**
 * @brief Check if the client is identified and set Nickname and Username if needed
 *
 * @return false if error, true if ok
 */
bool Client::checkIdentification()
{
	if (_id.certify)
		return (true);
	if (_id.Username.length() > 0 && _id.Nickname.length() > 0 && (_id.validPassword || _server->checkPassword(""))) {
		std::cout << "Client " << _ip << " is now identified as " << _id.Nickname << std::endl;
		sendMessage("001 RPL_WELCOME :Welcome to the Internet Relay Network " + _id.Nickname);
		_id.certify = true;
	}
	return (_id.certify);
}
