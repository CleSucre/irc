#pragma once

#include "Bot.hpp"

class Client
{
	private:
		std::string _nick;
		std::string _username;
		size_t _id;
		size_t _last_message;
	public:
		Client();
		Client(const std::string &, const std::string &, size_t id);
		Client(const Client &old);
		Client &operator=(const Client &old);
		~Client();




};