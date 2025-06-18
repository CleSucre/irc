#include "Bot.hpp"


/**
 * @brief Main function to connect to an IRC server
 * @param argc Number of command line arguments
 * @param argv Array of command line arguments
 * @return 0 on success, 1 on error
 */
int main(int argc, char* argv[]) {

    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <server> [port] [pass] [nick] [username]" << std::endl;
        return 1;
    }
    Bot bot;
    if (bot.socket_creation(argc, argv))
        return 1;
    return 0;
}
