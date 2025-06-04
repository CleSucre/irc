#include "Bot.hpp"

Bot::Bot() : _sock(-1), _nick("bot"), _current_channel(0), _channel_known(0) {}

Bot::~Bot()
{
	if (_sock != -1) {
		close(_sock);
	}
}

/**
 * @brief Copy a list of strings to a vector of pairs
 * This function copies unique strings from the source vector to the destination vector,
 * ensuring that each string is paired with a size_t value initialized to is id.
 * @param src Source vector of strings
 * @param dest Destination vector of pairs (string, size_t)
 */
void copy_list(std::vector<std::string> &src, std::vector<std::pair<std::string, size_t> > &dest)
{

	for (std::vector<std::string>::iterator it = src.begin(); it != src.end(); ++it)
	{
		if(it->empty())
			continue;
		bool match = false;
		for(std::vector<std::pair<std::string, size_t> >::iterator jt = dest.begin(); jt != dest.end(); ++jt)
		{
			if (jt->first == *it)
			{
				match = true;
				break ;
			}
		}
		if (!match)
			dest.push_back(std::make_pair(*it, dest.size()));
	}
}

/**
 * @brief List channels on the IRC server
 * This function sends a LIST command to the IRC server and processes the response.
 * It updates the list of known channels and prints new channels found.
 */
void Bot::list_channels()
{
	size_t channel_nb = 0;
	char buffer[512] = {0};
	std::vector<std::string> channels;
	
	std::cout << "Get server list\n";
	send(_sock, "LIST\r\n", 6, 0);

	// 323 is the response code for "End of /LIST command"
	// It could be change by END or other code, but 323 is the most common
	while (strstr(buffer, "323") == NULL)
	{
		int len = recv(_sock, buffer, sizeof(buffer)-1, 0);
		if (len <= 0)
		{
		    std::cerr << "Connection closed or error occurred\n";
		    break;
		}
		buffer[len] = '\0';
		channel_nb++;
		std::cout << "Received: " << buffer << std::endl;
		std::cout << buffer;
		channels.push_back(std::string(buffer));
	}
	if (channel_nb > _channel_known)
	{
		std::cout << "New channels found: " << channel_nb - _channel_known << "\n";
		copy_list(channels, _channel_list);
	}
	else
		std::cout << "No new channels found.\n";
}

/**
 * @brief Loop to communicate with the IRC server
 * This function handles sending and receiving messages, checking for new channel,
 *  clients, and processing incoming messages.
 */
void Bot::communication_loop()
{
    // Loop to send LIST, USER and check PRIVMSG
    while (true) {
		sleep(2);
       
		// Check client by WHO

		// Check if more client than already in vector 

		// ADD all client by channel in vector

		// Check messages

		char buffer[512];
        std::string line(buffer);
        if (line.find("PRIVMSG :") == 0) {
            std::cout << "Message receive : " << line << std::endl;
            // std::string token = line.substr(6);
            // std::string pong = "PONG :" + token + "\r\n";
            // send(sock, pong.c_str(), pong.size(), 0);
            // std::cout << ">> " << pong;
        }
		else
			std::cout << "No message received\n";
	}
    return ;
}

void Bot::server_authentification()
{
    std::string nickCmd = "NICK " + std::string(_nick) + "\r\n";
    std::string userCmd = "USER " + std::string(_nick) + " 0 * :Server bot\r\n";
    std::cout << "NICK command sent\n" << std::endl;
    send(_sock, nickCmd.c_str(), nickCmd.size(), 0);
	usleep(500);
    std::cout << "USER command sent\n" << std::endl;
    send(_sock, userCmd.c_str(), userCmd.size(), 0);
    usleep(500);
    std::cout << "Authentication complete, entering communication loop...\n" << std::endl;
    communication_loop();


}

/**
 * @brief Create a socket and connect bot to the IRC server
 * @param argc Number of command line arguments
 * @param argv Array of command line arguments
 * @return 0 on success, 1 on error
 */
int Bot::socket_creation(int argc, char* argv[])
{
    const char* server = (argc > 1 ? argv[1] : "127.0.0.1");
    int port          = (argc > 2 ? std::atoi(argv[2]) : 6667);
    _nick   = (argc > 3 ? argv[3] : "bot");

    _sock = socket(AF_INET, SOCK_STREAM, 0);
    if (_sock < 0) {
        std::cerr << "Socket Error\n";
        return 1;
    }
    sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    if (inet_pton(AF_INET, server, &addr.sin_addr) <= 0) {
        std::cerr << "Invalid adress\n";
        return 1;
    }

    if (connect(_sock, (sockaddr*)&addr, sizeof(addr)) < 0) {
        std::cerr << "Impossible to connect to " << server << ":" << port << "\n";
        return 1;
    }
    std::cout << "Connected to " << server << ":" << port << "\n";
    server_authentification();
    close(_sock);
    return (0);
}