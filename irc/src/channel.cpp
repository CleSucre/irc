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

bool Channel::getMode(int index) {
	return _mode[index];
}

void Channel::setMode(int index, bool mode, unsigned int optionnal) {
	if (index == mL && mode == true) {
		_modeL = optionnal;
	}
	_mode[index] = mode;
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
		if (it->second != user)
			return false;
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

	for (std::vector<std::pair<Client*, int> >::iterator it = _user.begin(); it != _user.end(); ++it) {
		if (it->first == client) {
			it->second = user;
			return true;
		}
	}

	_user.push_back(std::make_pair(client, user));
	return true;
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

bool Channel::kickUser(Client* client, Client* toKick) {
	if (!client || !toKick || client == toKick)
		return false;

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
				return true; // :client->getNickname() KICK #channel target :reason
			} else {
				return false; // 482 ERR_CHANOPRIVSNEEDED "<channel> :You're not channel operator"
			}
		} else {
			return false; // 442 ERR_NOTONCHANNEL "<channel> :You're not on that channel"
		}

	}
	return false; // 441 ERR_USERNOTINCHANNEL "<nick> <channel> :They aren't on that channel"
}

bool Channel::isMember(Client* client) {
	for (std::vector<std::pair<Client*, int> >::const_iterator it = _user.begin(); it != _user.end(); ++it) {
		if (it->first == client)
			return true;
	}
	return false;
} // revoir si j'appose un filtre pour eviter de checker les guess

// question sour l'implementation de la gestion d'erreur dans les methode de channel
