#include "Bot.hpp"

int Bot::_end_signal = 0;


Bot::Bot() : _sock(-1), _nick("bot"), _current_channel(0), _channel_known(0) {
	_ping_status._last_ping = std::time(NULL);
	_ping_status._waiting_pong = false;
}

Bot::~Bot()
{
	if (_sock != -1)
		close(_sock);
}

/**
 * @brief Loop to communicate with the IRC server
 * This function handles sending and receiving messages, checking for new channel,
 *  clients, and processing incoming messages.
 * It uses the select function to wait for incoming data on the socket.
 * @note
 * The communication loop is the main part of the bot that keeps it running and interacting with the IRC server.
 * It is responsible for maintaining the connection, processing incoming messages, and sending commands.
 * It uses a timeout to check for new channels and users every 5 seconds.
 * It can be extended to handle more complex interactions with the IRC server, such as responding to specific commands or messages.
 */
void Bot::communication_loop()
{
	fd_set master_fds, read_fds;
	FD_ZERO(&master_fds);
	FD_SET(_sock, &master_fds);

	struct timeval timeout;
	timeout.tv_sec = 1;
	timeout.tv_usec = 0;
	int max_fd = _sock;
	time_t _last_check = std::time(NULL);
	time_t last_check_who = std::time(NULL);
    while (_end_signal == 0) {
		time_t current_time = std::time(NULL);

		if (_ping_status._waiting_pong == false && current_time - _ping_status._last_ping >= _ping_status._ping_delay)
		{
			std::cout << "Sending PING command to server..." << std::endl; // TODO: Debug message
			_ping_status._last_ping = current_time;
			_ping_status.tokens = current_time;
			std::string pingCmd = "PING :" + _ping_status.tokens + "\r\n";
			send(_sock, pingCmd.c_str(), pingCmd.size(), 0);
			_ping_status._waiting_pong = true;
		}
		if (_last_check == 0 || current_time - _last_check >= check_interval)
		{

			std::cout << "Checking for new channels and users..." << std::endl; // TODO: Debug message
				send(_sock, "LIST\r\n", 6, 0);
			_last_check = current_time;
			usleep(100);
		}
		if (current_time - last_check_who >= check_who_interval)
		{
			std::cout << "Checking for users in channels..." << std::endl; // TODO: Debug message
			user_command();
			last_check_who = current_time;
		}
		read_fds = master_fds;
		if (select(max_fd + 1, &read_fds, NULL, NULL, &timeout) < 0) {
			std::cerr << "Select error\n";
			return;
		}
		if (FD_ISSET(_sock, &read_fds))
			handle_global_data();
		if (_ping_status._waiting_pong && current_time - _ping_status._last_ping >= _ping_status._pong_waiting_delay)
		{
			std::cerr << "No PONG received, server may be down or not responding." << std::endl;
			_end_signal = 1;
		}
	}
    return ;
}

void Bot::server_authentification()
{
    std::string nickCmd = "NICK " + std::string(_nick) + "\r\n";
    std::string userCmd = "USER " + std::string(_username) + " 0 * :" + _username + "\r\n";
	std::string passCmd = "PASS " + std::string(_password) + "\r\n";
    std::cout << "NICK command sent" << std::endl;
    send(_sock, nickCmd.c_str(), nickCmd.size(), 0);
	usleep(100);
    std::cout << "USER command sent" << std::endl;
    send(_sock, userCmd.c_str(), userCmd.size(), 0);
    usleep(100);
	std::cout << "PASS command sent" << std::endl;
	send(_sock, passCmd.c_str(), passCmd.size(), 0);
    std::cout << "Authentication complete, entering communication loop..." << std::endl;
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
	_password = (argc > 3 ? argv[3] : "\0");
    _nick   = (argc > 4 ? argv[4] : "bot");
	_username = (argc > 5 ? argv[5] : "Serverbot");

	signal(SIGINT, Bot::handleSignal);
	signal(SIGQUIT, Bot::handleSignal); 

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