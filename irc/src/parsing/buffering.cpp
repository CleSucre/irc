//struct pollfd fds[MAX_CLIENTS];
// This is a pollfd type from ChatGPT to help me imagine my partner code
// struct pollfd {
//     int   fd;        // fd to watch
//     short events;    // event to watch (ex. POLLIN)
//     short revents;   // events occureds (fill by poll)
// };

/** 
 * @brief Got a fd's tab from partner A
 *  theses fds are set to ok to listen (POLLIN) by poll() functions
 *  Need to parse theses tabs
 */

/**
 * Get pollfd
 * 	if (poll_fd = POLLIN)
 * {
 * 		client_fd= poll_fd->fd
 * 		recv (client_fd, client->buffer, sizeof(client->buffer), 0)
 * 		
 * }
 */