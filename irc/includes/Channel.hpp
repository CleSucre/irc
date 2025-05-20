#pragma once

#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <algorithm>
#include "colors.h"
#include "Client.hpp"
#include "Server.hpp"

#define mI 0
#define mT 1
#define mK 2
#define mO 3
#define mL 4

#define admin 2
#define user 1
#define guess 0

class Channel {
	private:

		std::string		_name;
		std::string		_password;
		std::string		_topic;
		bool			_mode[5];
		unsigned int	_modeL;
		std::vector<std::pair<Client *, int> > _user;

	public:

		Channel(std::string n);
		Channel(std::string n, std::string p);
		~Channel();

		std::string getName();

		std::string getPassword();
		void setPassword(std::string p);

		std::string getTopic();
		void setTopic(std::string t);

		bool getMode(int index);
		void setMode(int index, bool mode, unsigned int optionnal);

		std::vector<Client *> getAdmin();
		bool addAdmin(Client *client);

		std::vector<Client *> getUser();
		bool addUser(Client *client);

		std::vector<Client *> getGuess();
		bool addGuess(Client *client);

		bool kickUser(Client* client, Client* toKick);

		bool isMember(Client* client);
};


// void kick(const Client& client, std::vector<std::string>& cmd);
// void invite(const Client& client, std::vector<std::string>& cmd);
// void mode(const Client& client, std::vector<std::string>& cmd);
// void topic(const Client& client, std::vector<std::string>& cmd);
// void join(const Client& client, std::vector<std::string>& cmd);
// void privmsg(const Client& client, std::vector<std::string>& cmd);

