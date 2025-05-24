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

void Channel::setMode(int index, bool mode) {
	_mode[index] = mode;
}

void Channel::setModeL(unsigned int limit) {
	_modeL = limit;
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

int Channel::kickUser(Client* client, Client* toKick) {
	if (!client || !toKick || client == toKick)
		return 0; // possible de gerer differment si on veux

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
		return 0; // possible de gerer differment si on veux

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
			break ;
		}
	}
}

bool Channel::isEmpty() {
	return _user.empty();
}

unsigned int Channel::getSize() {
	return _user.size();
}

void Channel::broadcast(const Client& sender, const std::string& message) {
    for (std::vector<std::pair<Client*, int> >::const_iterator it = _user.begin(); it != _user.end(); ++it) {
        if (it->first && it->first != &sender) {
            it->first->sendMessage(message);
        }
    }
}
