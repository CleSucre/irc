#ifndef UTILS_HPP
#define UTILS_HPP

#include <iostream>
#include <sstream>
#include <vector>

std::vector<std::string> split(const std::string& str, char delimiter);
std::string joinFirstN(const std::vector<std::string>& vec, size_t n, const std::string& delimiter = " ");

#endif // UTILS_HPP
