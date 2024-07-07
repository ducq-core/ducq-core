#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <stdint.h>
#include <cmocka.h>

#include "../src/inet_socket.h"

#include <stdio.h>
#include <string.h>


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
int inet_close(int fd) {
	return mock();
}
int inet_shutdown_write(int fd) {
	check_expected(fd);
	return mock();
}
int inet_set_read_timeout(int fd, unsigned timeout_sec) {
	return mock();
}
int inet_reuseaddr(int fd) {
	return mock();
}
int inet_tcp_connect(const char *host, const char *service, int reuseaddr) {
	if(reuseaddr) {
		int ret = inet_reuseaddr(-1);
		if(ret) return ret;
	}

	return mock();
}
char *inet_socket_tostring(int socket, int self, char *buffer, size_t len) {
	check_expected(self);
	len = snprintf(buffer, len, "__id__");
	return buffer;
}
