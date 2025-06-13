#pragma once

#include "Bot.hpp"

class Channel; 
class Client
{
	private:
		std::string _nick;
		std::string _username;
		size_t _last_message;
		int _warning_count;
	public:
		Client();
		Client(const std::string &, const std::string &, size_t id);
		Client(const Client &old);
		Client &operator=(const Client &old);
		~Client();

		const std::string &getNick() const;
		const std::string &getUsername() const;
		size_t getId() const;
		size_t getLastMessage() const;
		void setWarningCount(int count);
		int getWarningCount() const;
		void resetLastMessage();
};

