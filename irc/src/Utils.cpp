#include "Utils.hpp"

std::vector<std::string> split(const std::string& str, char delimiter) {
    std::vector<std::string> tokens;
    std::string token;
    std::istringstream iss(str);

    while (std::getline(iss, token, delimiter)) {
        tokens.push_back(token);
    }

    return tokens;
}

std::string joinFirstN(const std::vector<std::string>& vec, size_t n, const std::string& delimiter) {
    std::ostringstream oss;
    for (size_t i = 0; i < n && i < vec.size(); ++i) {
        if (i > 0) {
            oss << delimiter;
        }
        oss << vec[i];
    }
    return oss.str();
}

std::string substr(const std::string& str, size_t start, size_t length) {
    std::string result = "";

    if (start >= str.length())
        return result;

    if (start + length > str.length())
        length = str.length() - start;

    for (size_t i = 0; i < length; ++i)
        result += str[start + i];

    return result;
}