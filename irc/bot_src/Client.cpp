#include "Client.hpp"

Client::Client() : _nick(""), _username(""), _id(0), _last_message(0) {}

Client::Client(const std::string &nick, const std::string &username, size_t id)
	: _nick(nick), _username(username), _id(id), _last_message(0) {}

Client::Client(const Client &old)
	: _nick(old._nick), _username(old._username), _id(old._id), _last_message(old._last_message) {}

Client &Client::operator=(const Client &old) {
	if (this != &old) {
		_nick = old._nick;
		_username = old._username;
		_id = old._id;
		_last_message = old._last_message;
	}
	return *this;
}

Client::~Client() {
	_nick.clear();
	_username.clear();
	_id = 0;
	_last_message = 0;
}

