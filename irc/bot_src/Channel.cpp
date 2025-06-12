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
	std::cout << "Searching for channel: " << channel_name << std::endl; // TODO: Debug message
	for (size_t i = 0; i < channels.size(); ++i)
	{
		if (channels[i].getName() == channel_name)
		{
			std::cout << "Channel found at index: " << i << std::endl; // TODO: Debug message
			return (i);
		}
	}
	throw(std::runtime_error("Channel not found"));
}

Client *Channel::getClientbyNick(const std::string &nick) {
	for(std::vector<Client>::iterator it = _clients.begin(); it != _clients.end(); ++it) {
		if (it->getNick() == nick) {
			return &(*it);
		}
	}
	return NULL;
}

void Channel::setOp(bool op) {
	_op = op;
}

bool Channel::getOp() const {
	return _op;
}

Channel *Bot::getChannelbyName(const std::string &name) {
	for (std::vector<Channel>::iterator it = _channel.begin(); it != _channel.end(); ++it) {
		if (it->getName() == name) {
			return &(*it);
		}
	}
	return NULL;
}

Channel *Bot::getChannelbyId(size_t id) {
	for (std::vector<Channel>::iterator it = _channel.begin(); it != _channel.end(); ++it) {
		if (it->getId() == id) {
			return &(*it);
		}
	}
	std::cout << "Channel with ID " << id << " not found." << std::endl; // TODO: Debug message
	return NULL;
}

void Channel::addClient(const Client &client) {
	_clients.push_back(client);
	std::cout << "Client " << client.getNick() << " added to channel " << _name << std::endl; // TODO: Debug message
}