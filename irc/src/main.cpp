#include "irc.hpp"

int parsePort(const std::string& portStr) {
    if (!isdigit(portStr[0])) {
        throw std::invalid_argument("Port must be a number");
    }
    int port;
    std::istringstream(portStr) >> port;
    if (port < PORT_MIN || port > PORT_MAX) {
        std::ostringstream oss;
        oss << "Port must be between " << PORT_MIN << " and " << PORT_MAX;
        throw std::invalid_argument(oss.str());
    }
    return port;
}

std::string parsePassword(const std::string& password) {
    if (password.length() < PASSWORD_MIN_LENGTH) {
        std::ostringstream oss;
        oss << "Password must be between " << PASSWORD_MIN_LENGTH
            << " and " << PASSWORD_MAX_LENGTH << " characters";
    } else if (password.length() > PASSWORD_MAX_LENGTH) {
        std::ostringstream oss;
        oss << "Password must be between " << PASSWORD_MIN_LENGTH
            << " and " << PASSWORD_MAX_LENGTH << " characters";
        throw std::invalid_argument(oss.str());
    }
    return password;
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << RED << "Usage: " << argv[0] << " <port> [password] [cert.pem] [key.pem]" << RESET << std::endl;
        return 1;
    }

    int port;
    std::string password;
    try {
        port = parsePort(argv[1]);
        if (argc > 2) {
            password = parsePassword(argv[2]);
        } else {
            password = DEFAULT_PASSWORD;
        }
    } catch (const std::invalid_argument& e) {
        std::cerr << RED << "Error: " << e.what() << RESET << std::endl;
        return 1;
    }

    std::string certFile = argc > 3 ? argv[3] : DEFAULT_CERT_FILE;
    std::string keyFile = argc > 4 ? argv[4] : DEFAULT_KEY_FILE;

    Server server("PIPI HOUSE", port, password, certFile, keyFile);
    server.start();
    return 0;
}