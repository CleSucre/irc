#pragma once

#include "Bot.hpp"
#include <sstream>

typedef struct s_map {
	int packet_index;
	const char *code_name;
}	t_map;

class CodeMap {
	private:
		static const int _dictionnary_size = 3;
		t_map _entry[_dictionnary_size];
	public:
		CodeMap();
		~CodeMap();
		int getIndex(std::string packet);
};

std::vector<std::string> split(const std::string& str, char delimiter);