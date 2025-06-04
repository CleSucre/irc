#include "Channel.hpp"

Channel::Channel() : _name(""), _id(0) {}

Channel::Channel(const std::string &name, size_t id) : _name(name), _id(id) {}

Channel::Channel(const Channel &old) : _name(old._name), _id(old._id), _clients(old._clients) {}

Channel &Channel::operator=(const Channel &old) {
	if (this != &old) {
		_name = old._name;
		_id = old._id;
		_clients = old._clients;
	}
	return *this;
}

Channel::~Channel() {
	_clients.clear();
}

