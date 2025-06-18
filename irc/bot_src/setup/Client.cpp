#include "Client.hpp"

Client::Client() : _nick(""), _username(""), _id(0), _last_message(0), _warning_count(0) {}

Client::Client(const std::string &nick, const std::string &username, size_t id)
	: _nick(nick), _username(username), _id(id), _last_message(0), _warning_count(0) {}

Client::Client(const Client &old)
	: _nick(old._nick), _username(old._username), _id(old._id), _last_message(old._last_message), _warning_count(old._warning_count){}

Client &Client::operator=(const Client &old) {
	if (this != &old) {
		_nick = old._nick;
		_username = old._username;
		_id = old._id;
		_last_message = old._last_message;
		_warning_count = old._warning_count;
	}
	return *this;
}

Client::~Client() {
	_nick.clear();
	_username.clear();
	_id = 0;
	_last_message = 0;
}


const std::string &Client::getNick() const {
	return _nick;
}

const std::string &Client::getUsername() const {
	return _username;
}

size_t Client::getId() const {
	return _id;
}

size_t Client::getLastMessage() const {
	return _last_message;
}

void Client::setWarningCount(int count){
	_warning_count = count;
	if (_warning_count < 0) {
		_warning_count = 0;
	}
	return ;
}

int Client::getWarningCount() const {
	return _warning_count;
}

void Client::resetLastMessage() {
	_last_message = std::time(NULL);
}
