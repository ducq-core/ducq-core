#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <stdint.h>
#include <cmocka.h>

#include "../src/inet_socket.h"

#include <stdio.h>
#include <string.h>

int inet_close(int fd) {
	return mock();
}
int inet_tcp_connect(const char *host, const char *service) {
	return mock();
}
int inet_listen(const char *service, int backlog, socklen_t *addrlen) {
	
}
int inet_bind(const char *service, int type, socklen_t *addrlen) {
	
}
char *inet_address_tostring(const struct sockaddr *addr, socklen_t addrlen, char *buffer, size_t *len) {
	
}
char *inet_peer_socket_tostring(int socket, char *buffer, size_t *len) {
	function_called();
	
	*len = snprintf(buffer, *len, "__id__");
	return buffer;
}
char *inet_self_socket_tostring(int socket, char *buffer, size_t *len) {
	
}
char *inet_socket_tostring(int socket, int self, char *buffer, size_t *len) {
	
}



char READ_BUFFER[BUFSIZ] = {};
int  pos = 0;

ssize_t readn(int fd, void *vptr, size_t n) {
	memcpy(vptr, READ_BUFFER + pos, n);
	pos += n;

	return mock();
}



ssize_t writen(int fd, const void *vptr, size_t n) {
	check_expected(vptr);
	return mock();
}
int inet_set_read_timeout(int fd, unsigned timeout_sec) {
	return mock();
}