#include "CodeMap.hpp"

	// TODO: put define here

/**
 * @brief CodeMap constructor
 * Initializes the mapping of packet codes to their indices.
 */
CodeMap::CodeMap(){
	_entry[0] = {4, "322"};
	_entry[1] = {5, "352"};
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
