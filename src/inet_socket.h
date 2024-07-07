#ifndef INET_SOCKET_HEADER
#define INET_SOCKET_HEADER

#include <stddef.h>					// size_t

#ifdef __linux__
	#include <unistd.h>				// read, write, ssize_t
	#include <sys/socket.h>		// sockaddr, socklen_t
#elif ESP_PLATFORM
	#include "lwip/sockets.h"
#endif

ssize_t readn(int fd, void *vptr, size_t n);
ssize_t writen(int fd, const void *vptr, size_t n);

int inet_set_read_timeout(int fd, unsigned timeout_sec);
int inet_tcp_connect(const char *host, const char *service, int reuseaddr);
int inet_close(int fd);
int inet_shutdown_write(int fd);

char *inet_address_tostring(const struct sockaddr *addr, socklen_t addrlen, char *buffer, size_t len);
char *inet_socket_tostring(int socket, int self, char *buffer, size_t len);
#define inet_self_socket_tostring(socket, buffer, len) \
	inet_socket_tostring(socket, 1, buffer, len)
#define inet_peer_socket_tostring(socket, buffer, len) \
	inet_socket_tostring(socket, 0, buffer, len)


#endif // INET_SOCKET_HEADER
