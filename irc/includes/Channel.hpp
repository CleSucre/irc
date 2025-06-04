#pragma once

#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <algorithm>
#include "colors.h"
#include "Client.hpp"
#include "Server.hpp"
#include "Utils.hpp"
#include "Error.hpp"
#include "Response.hpp"

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

		bool getMode(int index) const;
		void setMode(int index, bool mode);
		unsigned int getModeL() const;
		void setModeL(unsigned int limit);

		std::string getModeString(bool isOperator) const;

		bool isAdmin(Client *client) const;
		std::vector<Client *> getAdmin();
		bool addAdmin(Client *client);

		bool isUser(Client *client) const;
		std::vector<Client *> getUser();
		bool addUser(Client *client);

		bool isGuess(Client *client) const;
		std::vector<Client *> getGuess();
		bool addGuess(Client *client);

		std::string getAllNicks() const;

		int kickUser(Client* client, Client* toKick);

		int inviteUser(Client* client, Client* toAdd);

		int getRole(Client *client);

		bool removeUser(Client* client);

		bool isEmpty();

		unsigned int getSize();

		unsigned int getNbrMember();

		void broadcast(const Client& sender, const std::string& message);
};

// void kick(Client* client, std::vector<std::string>& cmd);
// void invite(Client* client, std::vector<std::string>& cmd);
// void mode(Client* client, std::vector<std::string>& cmd);
// void topic(Client* client, std::vector<std::string>& cmd);
// void join(Client* client, std::vector<std::string>& cmd);
// void privmsg(Client* client, std::vector<std::string>& cmd);
// void part(Client* client, std::vector<std::string>& cmd);

// faire une fonction qui send un msg a tout les membre du channel ?
