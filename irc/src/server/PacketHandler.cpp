#include "irc.hpp"

/**
 * @brief Handles the reception of packets from clients
 * 
 * @param client The client that sent the packet
 * @param packet The packet received from the client
 */
void packetRecieption(const Client& client, const std::string& packet) {
    std::cout << "Packet received from client " << client.getFd() << ": " << packet << std::endl;
}
