#include "Bot.hpp"



// void Bot::communication_loop()
// {
//     char buffer[512];
//     // Loop to send LIST, USER and check PRIVMSG
//     while (true) {
//         std::cout << "Get server list\n";
//         send(_sock, "LIST\r\n", 6, 0);
//         int len = recv(_sock, buffer, sizeof(buffer)-1, 0);
//         if (len <= 0)
//         {
//             std::cerr << "Connection closed or error occurred\n";
//             break;
//         }
//         buffer[len] = '\0';
//         std::cout << "Received: " << buffer << std::endl;
//         std::cout << buffer;


//         std::string line(buffer);
//         if (line.find("PRIVMSG :") == 0) {
//             std::cout << "Message receive : " << line << std::endl;
//             // std::string token = line.substr(6);
//             // std::string pong = "PONG :" + token + "\r\n";
//             // send(sock, pong.c_str(), pong.size(), 0);
//             // std::cout << ">> " << pong;
//         }
//     }
//     return ;
// }

// void Bot::server_authentification()
// {
//     std::string nickCmd = "NICK " + std::string(_nick) + "\r\n";
//     std::string userCmd = "USER " + std::string(_nick) + " 0 * :Server bot\r\n";
//     std::cout << "NICK command sent\n" << std::endl;
//     send(_sock, nickCmd.c_str(), nickCmd.size(), 0);
// 	usleep(500);
//     std::cout << "USER command sent\n" << std::endl;
//     send(_sock, userCmd.c_str(), userCmd.size(), 0);
//     usleep(500);
//     std::cout << "Authentication complete, entering communication loop...\n" << std::endl;
//     communication_loop();


// }

// int Bot::socket_creation(int argc, char* argv[])
// {
//     const char* server = (argc > 1 ? argv[1] : "127.0.0.1");
//     int port          = (argc > 2 ? std::atoi(argv[2]) : 6667);
//     _nick   = (argc > 3 ? argv[3] : "bot");

//     _sock = socket(AF_INET, SOCK_STREAM, 0);
//     if (_sock < 0) {
//         std::cerr << "Socket Error\n";
//         return 1;
//     }
//     sockaddr_in addr;
//     addr.sin_family = AF_INET;
//     addr.sin_port = htons(port);
//     if (inet_pton(AF_INET, server, &addr.sin_addr) <= 0) {
//         std::cerr << "Invalid adress\n";
//         return 1;
//     }

//     if (connect(_sock, (sockaddr*)&addr, sizeof(addr)) < 0) {
//         std::cerr << "Impossible to connect to " << server << ":" << port << "\n";
//         return 1;
//     }
//     std::cout << "Connected to " << server << ":" << port << "\n";
//     server_authentification();
//     close(_sock);
//     return (0);
// }

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
    Bot bot;
    if (bot.socket_creation(argc, argv))
        return 1;



    // char buffer[512];
    // // Loop to send LIST, USER and check PRIVMSG
    // while (true) {
    //     std::cout << "Get server list\n";
    //     send(_sock, "LIST\r\n", 6, 0);
    //     int len = recv(_sock, buffer, sizeof(buffer)-1, 0);
    //     if (len <= 0) break;
    //     buffer[len] = '\0';
    //     std::cout << "Received: " << buffer << std::endl;
    //     std::cout << buffer;


    //     std::string line(buffer);
    //     if (line.find("PRIVMSG :") == 0) {
    //         std::cout << "Message receive : " << line << std::endl;
    //         // std::string token = line.substr(6);
    //         // std::string pong = "PONG :" + token + "\r\n";
    //         // send(sock, pong.c_str(), pong.size(), 0);
    //         // std::cout << ">> " << pong;
    //     }
    // }

    return 0;
}
