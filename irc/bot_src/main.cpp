#include "Bot.hpp"

#include <iostream>
#include <string>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>
#include <cstdlib>

/**
 * @brief Main function to connect to an IRC server
 * @param argc Number of command line arguments
 * @param argv Array of command line arguments
 * @return 0 on success, 1 on error
 */
int main(int argc, char* argv[]) {

    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <server> [port] [nick]\n";
        return 1;
    }
    const char* server = (argc > 1 ? argv[1] : "localhost");
    int port          = (argc > 2 ? std::atoi(argv[2]) : 6667);
    const char* nick   = (argc > 3 ? argv[3] : "bot");

    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        std::cerr << "Socket Error\n";
        return 1;
    }

    sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    if (inet_pton(AF_INET, server, &addr.sin_addr) <= 0) {
        std::cerr << "Adresse invalide\n";
        return 1;
    }

    if (connect(sock, (sockaddr*)&addr, sizeof(addr)) < 0) {
        std::cerr << "Impossible de se connecter à " << server << ":" << port << "\n";
        return 1;
    }
    std::cout << "Connecté à " << server << ":" << port << "\n";

    std::string nickCmd = "NICK " + std::string(nick) + "\r\n";
    std::string userCmd = "USER " + std::string(nick) + " 0 * :Server bot\r\n";
    std::cout << "NICK command sent\n" << std::endl;
    send(sock, nickCmd.c_str(), nickCmd.size(), 0);
	usleep(500);
    std::cout << "USER command sent\n" << std::endl;
    send(sock, userCmd.c_str(), userCmd.size(), 0);

    char buffer[512];
    while (true) {
        std::cout << "En attente de messages...\n";
        int len = recv(sock, buffer, sizeof(buffer)-1, 0);
        if (len <= 0) break;
        buffer[len] = '\0';
        std::cout << buffer;

        std::string line(buffer);
        if (line.find("PRIVMSG :") == 0) {
            std::string token = line.substr(6);
            std::string pong = "PONG :" + token + "\r\n";
            send(sock, pong.c_str(), pong.size(), 0);
            std::cout << ">> " << pong;
        }
    }

    close(sock);
    return 0;
}
