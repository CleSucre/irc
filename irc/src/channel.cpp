/* made by tao_la_puissance (dev trainee)*/

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
		if (it->first == client) {
			if (it->second != user)
				return false;
			it->second = admin;
			return true;
		}
	}

	return false;
	// _user.push_back(std::make_pair(client, admin)); // normalement inutile(je laisse je suis parano)
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

	std::string clientName = client->getName();
	for (std::vector<std::pair<Client*, int> >::iterator it = _user.begin(); it != _user.end(); ++it) {
		if (clientName == it->first->getName()) {
			if (it->first == client ) {
				it->second = user;
				return true;
			}
			return false; // erreur name deja present
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

	std::string clientName = client->getName();
	for (std::vector<std::pair<Client*, int> >::iterator it = _user.begin(); it != _user.end(); ++it) {
		if (clientName == it->first->getName())
			return false; // erreur name deja present
	}

	_user.push_back(std::make_pair(client, guess));
	return true;
}

void Channel::kickUser(Client* client) {
	for (std::vector<std::pair<Client*, int> >::iterator it = _user.begin(); it != _user.end(); ++it) {
		if (it->first == client) {
			_user.erase(it);
			break;
		}
	}

	// :client->getNickname() KICK #channel target :reason
}

// bool isMember(Client* client); // test si le client est deja present
// faire attention a pas renvoyer d'erreur si on essaye d'envoyer une invite a un client qui est deja dans le serveur
