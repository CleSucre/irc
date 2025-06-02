#include "Client.hpp"


/**
 * @brief Set the NickName of the client
 * 		  Do nothing if new NickName is empty
 * @param name : NickName to set
 * @return true if ok, false if error
 */
bool Client::setNick(const std::string &name)
{
    if (name.length() < 1)
	{
		std::cerr << "Client " << _fd << " has entered an empty NickName." << std::endl;
		sendMessage("ERR_INVALIDNICKNAME :Invalid NickName, your Nickname will not change\r\n");
		return false;
	}
	else if (name.length() > 10 || name.find(" ") != std::string::npos)
	{
		std::cerr << "ICI FDP" << std::endl;
		std::cerr  << "[" << name << "]" <<  std::endl;
		std::cerr << "Client " << _fd << " has entered a wrong NickName : " << name << std::endl;
		sendMessage("ERR_INVALIDNICKNAME :Invalid NickName, your Nickname will not change\r\n");
		return false;
	}
    //TODO: Check if new NickName isn't already taken in the server and in channels
	if (_server->getClientByNickname(name) != NULL)
	{
		std::cerr<< "Client " << _fd << " has entered a NickName already taken : " << name << std::endl;
		sendMessage("ERR_NICKNAMEINUSE :NickName already in use\r\n");
		return false;
	}
	if (_id.Nickname != name)
		_id.Nickname = name;
	std::cout << "Client " << _id.Username << " has entered a new NickName : " << _id.Nickname << std::endl;
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
static bool parsecolon(const std::string &args, std::string *res)
{
	std::string::size_type limit;
	limit = args.find(":");
	if (limit == std::string::npos)
	{
		*res = "ERR_NEEDMOREPARAMS :Not enough parameters";
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
			*res = "ERR_INVALIDPARAMS :Invalid username";
			return (false);
		}
		tokens.push_back(param);
	}
	if (count < 3)
	{
		*res = "ERR_NEEDMOREPARAMS :Not enough parameters";
		return (false);
	}
	for(size_t i = 3; i < tokens.size(); i++)
		(*res).append(tokens[i] + " ");
	(*res).append(args.substr(limit + 1));
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
		sendMessage("462 ERR_ALREADYREGISTERED :Unauthorized command (already registered)\r\n");
		std::cerr << "Client " << _id.Username << " is already registered." << std::endl;
		return (false);
	}
	std::string username;
	if (parsecolon(args, &username) == false)
	{
		std::cerr << "Client " << _fd << " " << username << std::endl;
		sendMessage(username + "\r\n");
		return (false);
	}
	if (username.length() < 1 || username.length() > 75)
	{
		sendMessage("ERR_INVALIDPARAMS :Invalid username\r\n");
		std::cerr << "Client " << _id.Username << " has entered a wrong UserName." << std::endl;
		return (false);
	}
	if (_server->getClientByName(username) != NULL)
	{
		std::cerr<< "Client " << _fd << " has entered a NickName already taken : " << username << std::endl;
		sendMessage("ERR_NICKNAMEINUSE :NickName already in use\r\n");
		return false;
	}
	//TODO: Check if new UserName isn't already taken in the server and in channels --- Same function as in Nickname
	_id.Username = username;
	std::cout << "Client " << _fd << " is now known as : " << _id.Username << std::endl;
    return (true);
}

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
		std::cerr << "Client " << _fd << " is not registered." << std::endl;
		sendMessage("ERR_NOTREGISTERED :You have not registered\r\n");
		return false;
	}
	if (_buff.find("NICK") != std::string::npos)
	{
		if (setNick(_buff.substr(_buff.find("NICK") + 5)) == false)
		{
			_buff.erase(0, _buff.size());
			return false;
		}
		_buff.erase(0, _buff.find("\r\n") + 2);
	}
	else if (_buff.find("USER") != std::string::npos)
	{
		if (setUser(_buff.substr(_buff.find("USER") + 5)) == false)
		{
			_buff.erase(0, _buff.size());
			return false;
		}
		_buff.erase(0, _buff.find("\r\n") + 2);
	}
	if (_id.Username.length() > 0  && _id.Nickname.length() > 0)
	{
		if (_id.certify == false)
			sendMessage("001 RPL_WELCOME :Welcome to the Internet Relay Network " + _id.Nickname);
		_id.certify = true;
	}
	return true;
}