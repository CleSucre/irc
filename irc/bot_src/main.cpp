#include "Bot.hpp"

// main.cpp
#include <iostream>
#include <string>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>
#include <cstdlib>

/**
 * @brief Main function to connect to an IRC server and respond to PING messages
 * @param argc Number of command line arguments
 * @param argv Array of command line arguments
 * @return 0 on success, 1 on error
 */
int main(int argc, char* argv[]) {
    const char* server = (argc > 1 ? argv[1] : "localhost");
    int port          = (argc > 2 ? std::atoi(argv[2]) : 6667);
    const char* nick   = (argc > 3 ? argv[3] : "bot");

    // 2) Socket creation
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        std::cerr << "Socket Error\n";
        return 1;
    }

    // 3) Server address setup
    sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    if (inet_pton(AF_INET, server, &addr.sin_addr) <= 0) {
        std::cerr << "Adresse invalide\n";
        return 1;
    }

    // 4) Server connection
    if (connect(sock, (sockaddr*)&addr, sizeof(addr)) < 0) {
        std::cerr << "Impossible de se connecter à " << server << ":" << port << "\n";
        return 1;
    }
    std::cout << "Connecté à " << server << ":" << port << "\n";

    // 5) Send NICK and USER commands
    std::string nickCmd = "NICK " + std::string(nick) + "\r\n";
    std::string userCmd = "USER " + std::string(nick) + " 0 * :Server bot\r\n";
    std::cout << "NICK command sent\n" << std::endl;
    send(sock, nickCmd.c_str(), nickCmd.size(), 0);
	usleep(500);
    std::cout << "USER command sent\n" << std::endl;
    send(sock, userCmd.c_str(), userCmd.size(), 0);

    // 6) Boucle de réception et réponse au PING
    char buffer[512];
    while (true) {
        std::cout << "En attente de messages...\n";
        int len = recv(sock, buffer, sizeof(buffer)-1, 0);
        if (len <= 0) break;
        buffer[len] = '\0';
        std::cout << buffer;

        std::string line(buffer);
        if (line.find("PING :") == 0) {
            std::string token = line.substr(6);
            std::string pong = "PONG :" + token + "\r\n";
            send(sock, pong.c_str(), pong.size(), 0);
            std::cout << ">> " << pong;
        }
    }

    close(sock);
    return 0;
}
