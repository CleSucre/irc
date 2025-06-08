#include "Channel.hpp"

Channel::Channel() : _name(""), _id(0), _op(false) {}

Channel::Channel(const std::string &name, size_t id) : _name(name), _id(id), _op(false) {}

Channel::Channel(const Channel &old) : _name(old._name), _id(old._id), _clients(old._clients), _op(old._op) {}

Channel &Channel::operator=(const Channel &old) {
	if (this != &old) {
		_name = old._name;
		_id = old._id;
		_clients = old._clients;
		_op = old._op;
	}
	return *this;
}

Channel::~Channel() {
	_clients.clear();
}

const std::string &Channel::getName() const {
	return _name;
}
size_t Channel::getId() const {
	return _id;
}

std::vector<Client> &Channel::getClientsList() {
	return _clients;
}

/**
 * @brief Find the index of a channel by its name in a vector of channels
 * @param channels The vector of channels to search
 * @param channel_name The name of the channel to find
 * @return The index of the channel if found
 * @throws std::runtime_error if the channel is not found
 */
size_t find_channel_index(const std::vector<Channel> &channels, const std::string &channel_name)
{
	for (size_t i = 0; i < channels.size(); ++i)
	{
		if (channels[i].getName() == channel_name)
			return (i);
	}
	throw(std::runtime_error("Channel not found"));
}

Client &Channel::getClientbyNick(const std::string &nick) {
	for(std::vector<Client>::iterator it = _clients.begin(); it != _clients.end(); ++it) {
		if (it->getNick() == nick) {
			return *it;
		}
	}
	throw std::runtime_error("Client not found");
}

void Channel::setOp(bool op) {
	_op = op;
}

bool Channel::getOp() const {
	return _op;
}