/* made by tao_la_puissance (dev trainee) */

#include "Channel.hpp"

Channel::Channel(std::string n) : _name(n) {
	std::fill(_mode, _mode + 5, false);
}

Channel::Channel(std::string n, std::string p) : _name(n) , _password(p) {
	std::fill(_mode, _mode + 5, false);
}

Channel::~Channel() {
	return ;
}

std::string Channel::getName() {
	return _name;
}

std::string Channel::getPassword() {
	return _password;
}

void  Channel::setPassword(std::string p) {
	_password = p;
}

std::string Channel::getTopic() {
	return _topic;
}
void Channel::setTopic(std::string t) {
	_topic = t;
}

bool Channel::getMode(int index) const {
	return _mode[index];
}

void Channel::setMode(int index, bool mode) {
	_mode[index] = mode;
}

unsigned int Channel::getModeL() const {
	return _modeL;
}

void Channel::setModeL(unsigned int limit) {
	_modeL = limit;
}

/**
 * @brief Get a string representation of the channel modes.
 *
 * @param isOperator Indicates if the client is an operator, if not, the password and user limit will not be included.
 * @return std::string The string representation of the channel modes.
 */
std::string Channel::getModeString(bool isOperator) const {
	std::string modeStr = "+";
	std::string params;

	if (_mode[mI])
		modeStr += "i";
	if (_mode[mT])
		modeStr += "t";
	if (_mode[mO])
		modeStr += "o";
	if (_mode[mK]) {
		modeStr += "k";
		if (isOperator)
			params += " " + _password;
	}
	if (_mode[mL]) {
		modeStr += "l";
		if (isOperator) {
			std::ostringstream oss;
			oss << _modeL;
			params += " " + oss.str();
		}
	}

	return modeStr + params;
}

bool Channel::isAdmin(Client *client) const {
	if (!client)
		return false;

	for (std::vector<std::pair<Client*, int> >::const_iterator it = _user.begin(); it != _user.end(); ++it) {
		if (it->first == client && it->second == admin)
			return true;
	}
	return false;
}

std::vector<Client *> Channel::getAdmin() {
	std::vector<Client *> tmp;
	for (std::vector<std::pair<Client*, int> >::const_iterator it = _user.begin(); it != _user.end(); ++it) {
		if (it->second == admin)
			tmp.push_back(it->first);
	}

	return tmp;
}

bool Channel::addAdmin(Client* client) {
	if (!client)
		return false;

	for (std::vector<std::pair<Client*, int> >::iterator it = _user.begin(); it != _user.end(); ++it) {
		if (it->first == client) {
			it->second = admin;
			return true;
		}
	}

	_user.push_back(std::make_pair(client, admin));
	return true;
}

bool Channel::isUser(Client *client) const {
	if (!client)
		return false;

	for (std::vector<std::pair<Client*, int> >::const_iterator it = _user.begin(); it != _user.end(); ++it) {
		if (it->first == client && it->second == user)
			return true;
	}
	return false;
}

std::vector<Client *> Channel::getUser() {
	std::vector<Client *> tmp;
	for (std::vector<std::pair<Client*, int> >::const_iterator it = _user.begin(); it != _user.end(); ++it) {
		if (it->second == user)
			tmp.push_back(it->first);
	}

	return tmp;
}

bool Channel::addUser(Client* client) {
	if (!client)
		return false;

	removeUser(client);

	for (std::vector<std::pair<Client*, int> >::iterator it = _user.begin(); it != _user.end(); ++it) {
		if (it->first == client) {
			it->second = user;
			return true;
		}
	}

	_user.push_back(std::make_pair(client, user));
	return true;
}

bool Channel::isGuess(Client *client) const {
	if (!client)
		return false;

	for (std::vector<std::pair<Client*, int> >::const_iterator it = _user.begin(); it != _user.end(); ++it) {
		if (it->first == client && it->second == guess)
			return true;
	}
	return false;
}

std::vector<Client *> Channel::getGuess() {
	std::vector<Client *> tmp;
	for (std::vector<std::pair<Client*, int> >::const_iterator it = _user.begin(); it != _user.end(); ++it) {
		if (it->second == guess)
			tmp.push_back(it->first);
	}

	return tmp;
}

bool Channel::addGuess(Client *client) {
	if (!client)
		return false;

	for (std::vector<std::pair<Client*, int> >::iterator it = _user.begin(); it != _user.end(); ++it) {
		if (it->first == client)
			return false;
	}

	_user.push_back(std::make_pair(client, guess));
	return true;
}

std::string Channel::getAllNicks() const {
	std::string names;
	for (std::vector<std::pair<Client*, int> >::const_iterator it = _user.begin(); it != _user.end(); ++it) {
		if (it->second == admin)
			names += "@" + it->first->getNick() + " ";
		else if (it->second == user)
			names += it->first->getNick() + " ";
		else if (it->second == guess)
			names += "+" + it->first->getNick() + " ";
	}

	if (!names.empty() && names[names.size() - 1] == ' ')
		names[names.size() - 1] = '\0';

	return names;
}

int Channel::kickUser(Client* client, Client* toKick) {
	if (!client || !toKick || client == toKick)
		return 0;

	std::vector<std::pair<Client*, int> >::iterator itToKick = _user.end();
	std::vector<std::pair<Client*, int> >::iterator itClient = _user.end();
	for (std::vector<std::pair<Client*, int> >::iterator it = _user.begin(); it != _user.end(); ++it) {
		if (it->first == toKick)
			itToKick = it;
		else if (it->first == client)
			itClient  = it;
	}

	if (itToKick != _user.end()) {
		if (itClient != _user.end()) {
			if (itClient->second == admin) {
				_user.erase(itToKick);
				return 0;
			} else {
				return 1;
			}
		} else {
			return 2;
		}

	}
	return 3;
}

int Channel::inviteUser(Client* client, Client* toAdd) {
	if (!client || !toAdd || client == toAdd)
		return 0;

	std::vector<std::pair<Client*, int> >::iterator itToAdd = _user.end();
	std::vector<std::pair<Client*, int> >::iterator itClient = _user.end();
	for (std::vector<std::pair<Client*, int> >::iterator it = _user.begin(); it != _user.end(); ++it) {
		if (it->first == toAdd)
			itToAdd = it;
		else if (it->first == client)
			itClient  = it;
	}

	if (itToAdd == _user.end()) {
		if (itClient != _user.end()) {
			if (itClient->second == admin) {
				_user.push_back(std::make_pair(toAdd, guess));
				return 0;
			} else {
				return 1;
			}
		} else {
			return 2;
		}

	}
	return 3;
}

int Channel::getRole(Client *client) {
	for (std::vector<std::pair<Client*, int> >::const_iterator it = _user.begin(); it != _user.end(); ++it) {
		if (it->first == client)
			return it->second;
	}
	return -1;
}

bool Channel::removeUser(Client* client) {
	for (std::vector<std::pair<Client*, int> >::iterator it = _user.begin(); it != _user.end(); ++it) {
		if (it->first == client){
			_user.erase(it);
			return true;
		}
	}
	return false;
}

bool Channel::isEmpty() {
	for (std::vector<std::pair<Client*, int> >::iterator it = _user.begin(); it != _user.end(); ++it) {
		if (it->second != guess)
			return false;
	}
	return true;
}

unsigned int Channel::getSize() {
	return _user.size();
}

unsigned int Channel::getNbrMember() {
	unsigned int nbr = 0;

	for (std::vector<std::pair<Client*, int> >::const_iterator it = _user.begin(); it != _user.end(); ++it) {
		if (it->second >= 1) {nbr++;}
	}
	return nbr;
}

void Channel::broadcast(const Client& sender, const std::string& message) {
	for (std::vector<std::pair<Client*, int> >::const_iterator it = _user.begin(); it != _user.end(); ++it) {
		if (it->first && it->first != &sender) {
			it->first->sendMessage(message);
		}
	}
}
