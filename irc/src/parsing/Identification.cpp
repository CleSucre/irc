#include "Client.hpp"
#include "Utils.hpp"

/**
 * @brief Set the NickName of the client
 * 		  Do nothing if new NickName is empty
 * @param name : NickName to set
 * @return true if ok, false if error
 */
bool Client::setNick(const std::string &name)
{
    if (name.length() < 1)
		return false;
	else if (name.length() > 10 || name.find(" ") != std::string::npos)
	{
		std::cerr << "Client " << _ip << " has entered a wrong NickName : " << name << std::endl;
		return false;
	}
    //TODO: Check if new NickName isn't already taken in the server and in channels
	if (_id.Nickname != name)
		_id.Nickname = name;
	std::cout << "Client " << _ip << " has entered a new NickName : " << _id.Nickname << std::endl;
	return true;
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
	//Check if the client is already registered
	if (_id.certify == true || !_id.Username.empty()) {
		sendMessage("462 ERR_ALREADYREGISTERED :Unauthorized command (already registered)\r\n");
		std::cerr << "Client " << _id.Username << " is already registered." << std::endl;
		return (false);
	}
	if (!isValidUsername(username)) {
		sendMessage("ERR_INVALIDPARAMS :Invalid username\r\n");
		std::cerr << "Client " << _id.Username << " has entered a wrong UserName." << std::endl;
		return (false);
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
	if (_id.Username.length() > 0 && _id.Nickname.length() > 0 && _id.validPassword)
	{
		std::cout << "Client " << _ip << " is now identified as " << _id.Nickname << std::endl;
		sendMessage("001 RPL_WELCOME :Welcome to the Internet Relay Network " + _id.Nickname + "\r\n");
		_id.certify = true;
	}
	return (_id.certify);
}
