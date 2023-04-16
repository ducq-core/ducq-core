#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>
#include <errno.h>


#include "inet_socket.h"

#ifdef __linux__
	#include <arpa/inet.h>	// inet_pton
#endif



ssize_t readn(int fd, void *vptr, size_t n) {
	char *ptr = (char*) vptr;
	size_t nleft = n;

	while(nleft > 0) {
		ssize_t nread = 0;

		if( (nread = read(fd, ptr, nleft)) < 0) {
			if(errno == EINTR)
				nread = 0;
			else
				return -1;
		}

		else if(nread == 0)
			break;

		nleft -= nread;
		ptr += nread;
	}

	return n - nleft;
}


ssize_t writen(int fd, const void *vptr, size_t n) {
	char *ptr = (char*) vptr;
	size_t nleft = n;

	while(nleft > 0) {
		ssize_t nwritten = 0;
		if( (nwritten = write(fd, ptr, nleft)) <= 0) {
			if(nwritten < 0 && errno == EINTR)
				continue;
			else
				return -1;
		}

		nleft -= nwritten;
		ptr += nwritten;
	}

	return n;
}




int inet_set_read_timeout(int fd, unsigned timeout_sec) {
	struct timeval tv = {
		.tv_sec = timeout_sec,
		.tv_usec = 0
	};

	return setsockopt(fd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));
}

int inet_tcp_connect(const char *host, const char *service) {
	int fd = socket(AF_INET, SOCK_STREAM, 0);
	if( fd == -1)
		return  -1;

	struct sockaddr_in addr = {
		.sin_family = AF_INET,
		.sin_port   = htons(atoi(service)),
	};
	if(inet_pton(AF_INET, host, &addr.sin_addr) == -1)
		return -1;

	if(connect(fd, (struct sockaddr*)&addr, sizeof(addr)) == -1)
		return -1;

	return fd;
}

int inet_close(int fd) {
	return close(fd);
}







char *inet_address_tostring(const struct sockaddr *addr, socklen_t addrlen, char *buffer, size_t len) {
	const char *ptr = NULL;
	in_port_t port  = 0;

	switch(addr->sa_family) {
		case AF_INET: {
			struct sockaddr_in *addr_in = (struct sockaddr_in *)addr;
			ptr  = inet_ntop(AF_INET, &addr_in->sin_addr, buffer, len);
			port = ntohs(addr_in->sin_port);
			break;
		}
		case AF_INET6: {
			struct sockaddr_in6 *addr_in6 = (struct sockaddr_in6 *)addr;
			ptr  = inet_ntop(AF_INET6, &addr_in6->sin6_addr, buffer, len);
			port = ntohs(addr_in6->sin6_port);
			break;
		}
		default: return NULL;
	}

	if(! ptr) return NULL;

	ptr += strlen(ptr);
	size_t max = len - (ptr-buffer);
	int n = snprintf((char*)ptr, max, ":%d", port); // discard const
	if(0 >= n || n >= max)
		return NULL;

	return buffer;
}


char *inet_socket_tostring(int socket, int self, char *buffer, size_t len) {
	struct sockaddr_storage _addr;
	struct sockaddr* addr = (struct sockaddr*) &_addr;
	socklen_t socklen = sizeof(_addr);

	int (*getname)(int, struct sockaddr *, socklen_t*);
	getname = self ? getsockname : getpeername;
	if( -1 == getname(socket, addr, &socklen) )
		return NULL;

	if( NULL == inet_address_tostring(addr, socklen, buffer, len) )
		return NULL;

	return buffer;
}
