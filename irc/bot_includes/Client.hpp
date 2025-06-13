#pragma once

#include "Bot.hpp"

#define OPERATOR 1
#define GUEST 2

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

/**
 * @brief Structure to hold message information
 * This structure holds the username, channel, and message content.
 * It is used to store the parsed information from a packet message.
 * @note
 * The structure is used in the message_reception function to process incoming messages.
 * It is designed to be simple and efficient for storing message data.
 * std::string message is used to store the message content, it is here if we want to add more information later.
 */
struct t_message {
	std::string username;
	std::string channel;
	std::string message;
};
