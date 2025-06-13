#include "CodeMap.hpp"

/**
 * @brief CodeMap constructor
 * Initializes the mapping of packet codes to their indices.
 * 322 corresponds to the list of channels,
 * 352 corresponds to the list of users in a channel.
 */
CodeMap::CodeMap(){
	_entry[0].code_name = CHANNEL_START;
	_entry[0].packet_index = 3;
	_entry[1].code_name = WHO_START;
	_entry[1].packet_index = 5;
	_entry[2].code_name = LIST_START;
	_entry[2].packet_index = 4; 
}

CodeMap::~CodeMap(){}

/**
 * @brief Get the index of a packet based on its code name
 * @param packet The packet to search for
 * @return The index of the packet if found, -1 otherwise
 * This function searches through the dictionary of codes and returns the index
 * of the first code that matches the packet's code name.
 */
int CodeMap::getIndex(std::string packet)
{
	for(int i = 0; i < _dictionnary_size; i++)
	{
		if (packet.find(_entry[i].code_name) != std::string::npos)
		{
			return _entry[i].packet_index;
		}
	}
	return -1;
}

/**
 * @brief Split a string into tokens based on a delimiter
 * @param str The string to split
 * @param delimiter The character used to split the string
 * @return A vector of strings containing the split tokens
 */
std::vector<std::string> split(const std::string& str, char delimiter) {
    std::vector<std::string> tokens;
    std::string token;
    std::istringstream iss(str);

    while (std::getline(iss, token, delimiter)) {
        tokens.push_back(token);
    }

    return tokens;
}
