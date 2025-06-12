#pragma once

#include "Bot.hpp"

class Client;

class Channel
{
	private:
		std::string _name;
		size_t _id;
		std::vector<Client> _clients;
		bool 	_op;
	public:
		Channel();
		Channel(const std::string &name, size_t id);
		Channel(const Channel &old);
		Channel &operator=(const Channel &old);
		~Channel();
		const std::string &getName() const;
		size_t getId() const;
		std::vector<Client> &getClientsList();
		Client *getClientbyNick(std::string const &nick);
		void addClient(const Client &client);
		void setOp(bool op);
		bool getOp() const;




};

size_t find_channel_index(const std::vector<Channel> &channels, const std::string &channel_name);