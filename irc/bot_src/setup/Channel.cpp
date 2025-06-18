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

/**
 * @brief Get a client by its nickname from the channel's client list
 * @param nick The nickname of the client to find
 * @return A pointer to the Client object if found, otherwise NULL
 * @note
 * This function iterates through the channel's client list and returns a pointer to the Client object
 * with the matching nickname. If no client with the given nickname is found, it returns NULL.
 */
Client *Channel::getClientbyNick(const std::string &nick) {
	for(std::vector<Client>::iterator it = _clients.begin(); it != _clients.end(); ++it) {
		if (it->getNick() == nick) {
			return &(*it);
		}
	}
	return NULL;
}

/**
 * @brief Set the operator status of the channel
 * @param op The operator status to set (true for operator, false for non-operator)
 * @note
 * This function sets the operator status of the channel. It can be used to promote or demote the bot
 * as an operator in the channel.
 */
void Channel::setOp(bool op) {
	_op = op;
}

/**
 * @brief Get the operator status of the channel
 * @return true if the channel is an operator channel, false otherwise
 */
bool Channel::getOp() const {
	return _op;
}

/**
 * @brief Get a channel by its name
 * @param name The name of the channel to find
 * @return A pointer to the Channel object if found, otherwise NULL
 * @note
 * This function iterates through the vector of channels and returns a pointer to the Channel object
 * with the matching name. If no channel with the given name is found, it returns NULL.
 */
Channel *Bot::getChannelbyName(const std::string &name) {
	for (std::vector<Channel>::iterator it = _channel.begin(); it != _channel.end(); ++it) {
		if (it->getName() == name) {
			return &(*it);
		}
	}
	return NULL;
}

/**
 * @brief Get a channel by its ID
 * @param id The ID of the channel to find
 * @return A pointer to the Channel object if found, otherwise NULL
 * @note
 * This function iterates through the vector of channels and returns a pointer to the Channel object
 * with the matching ID. If no channel with the given ID is found, it returns NULL.
 */
Channel *Bot::getChannelbyId(size_t id) {
	for (std::vector<Channel>::iterator it = _channel.begin(); it != _channel.end(); ++it) {
		if (it->getId() == id) {
			return &(*it);
		}
	}
	return NULL;
}

/**
 * @brief Add a client to the channel
 * @param client The Client object to add to the channel
 * @note
 * This function adds a Client object to the channel's client list. It can be used to add new clients
 * that join the channel or to update the client list with existing clients.
 */
//TODO: Check if function used
void Channel::addClient(const Client &client) {
	_clients.push_back(client);
	std::cout << "Client " << client.getNick() << " added to channel " << _name << std::endl; // TODO: Debug message
}
