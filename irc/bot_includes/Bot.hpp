#ifndef BOT_HPP
	#define BOT_HPP

#include <cstdlib>
#include <string>
#include <ctime>
#include <vector>


class Bot
{
	private:
		size_t current_channel;
		std::vector<std::string, size_t> flood_timer;
        std::vector<size_t, size_t> client_by_channel; 

	public:
		Bot();
		~Bot();
		
};

#endif