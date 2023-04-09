#ifndef INET_SOCKET_HEADER
#define INET_SOCKET_HEADER

#include <stddef.h>

#ifdef __linux__
	#include <unistd.h>      // ssize_t
	#include <sys/socket.h>  // sockaddr, socklen_t
#elif ESP_PLATFORM
	#include "lwip/sockets.h"
#endif


int inet_close(int fd);
int inet_tcp_connect(const char *host, const char *service);
int inet_listen(const char *service, int backlog, socklen_t *addrlen);
int inet_bind(const char *service, int type, socklen_t *addrlen);

char *inet_address_tostring(const struct sockaddr *addr, socklen_t addrlen, char *buffer, size_t *len);
char *inet_peer_socket_tostring(int socket, char *buffer, size_t *len);
char *inet_self_socket_tostring(int socket, char *buffer, size_t *len);
char *inet_socket_tostring(int socket, int self, char *buffer, size_t *len);

ssize_t readn(int fd, void *vptr, size_t n);
ssize_t writen(int fd, const void *vptr, size_t n);

int inet_set_read_timeout(int fd, unsigned timeout_sec);



#endif // INET_SOCKET_HEADER