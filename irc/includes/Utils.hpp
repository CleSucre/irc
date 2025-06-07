#ifndef UTILS_HPP
#define UTILS_HPP

#include <iostream>
#include <sstream>
#include <vector>
#include <string>

std::vector<std::string> split(const std::string& str, char delimiter);
std::string joinFirstN(const std::vector<std::string>& vec, size_t n, const std::string& delimiter = " ");
std::string substr(const std::string& str, size_t start, size_t length);

#endif // UTILS_HPP
