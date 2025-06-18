#include "Bot.hpp"


void Bot::handleSignal(int sig)
{
    if (sig == SIGINT || sig == SIGQUIT)
        Bot::_end_signal = 1;
}


/**
 * @brief Handle global data received from the IRC server
 * This function receives data from the IRC server and processes it.
 * It checks the type of packet received and calls the appropriate handler function.
 * It handles LIST and WHO commands responses, and can be extended to handle other types of packets.
 * @note
 * This function is called in the communication loop to handle incoming data from the server.
 * It uses the recv function to read data from the socket and processes it based on the packet type.
 * It currently handles LIST (322) and WHO (352, 315) commands responses.
 * It can be extended to handle other types of packets as needed.
 * @example:
 * Example of a packet received:
 * :irc.example.com 322 foo_user #canal 10 :This is a test channel
 */
void Bot::handle_global_data()
{
	char buffer[512] = {0};
	int len = recv(_sock, buffer, sizeof(buffer)-1, 0);
	if (len <= 0) {
		std::cerr << "Connection closed or error occurred\n";
		_end_signal = 1;
		return;
	}
	buffer[len] = '\0';
	std::string packet(buffer);
	if (packet.find(LIST_START) != std::string::npos)
		list_channels_handler(packet);
	else if (packet.find(WHO_START) != std::string::npos)
		list_users_handler(packet);
	else if (packet.find("PRIVMSG") != std::string::npos)
		message_reception(packet);
	else if (packet.find("MODE") != std::string::npos)
		operator_modification(packet);
	else if (_ping_status._waiting_pong && packet.find("PONG :" + _ping_status.tokens) != std::string::npos)
		_ping_status._waiting_pong = false;
	else if (packet.find("433") != std::string::npos)
		_end_signal = 1;
}

