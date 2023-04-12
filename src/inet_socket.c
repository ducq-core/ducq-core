#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdbool.h>
#include <ctype.h>
#include <errno.h>


#include "inet_socket.h"

#ifdef __linux__
	#include <sys/time.h>
	#include <netinet/in.h>
#elif ESP_PLATFORM
	#include "lwip/sockets.h"
#endif 


ssize_t readn(int fd, void *vptr, size_t n) {
	char *ptr = (char*) vptr;
	size_t nleft = n;

	while(nleft > 0) {
		size_t nread = 0;
		
		if( (nread = read(fd, ptr, nleft)) < 0) {
			if(errno == EINTR)
				nread = 0;
			else
				return -1;
		}
		
		else if(nread == 0) // EOF
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


static
bool _is_all_number(const char *str) {
	while(*str) {
		if( ! isdigit(*str) )
			return false;
		str++;
	}
	return true;
}


int inet_tcp_connect(const char *host, const char *service) {
	if(! _is_all_number(service))
		return -1;

	int fd = socket(AF_INET, SOCK_STREAM, 0);
	if( fd == -1) {
		return -1;
	}


	struct sockaddr_in addr = {
		// .sin_len    = sizeof(struct sockaddr_in),
		.sin_family = AF_INET,
		.sin_port   = htons(atoi(service)),
	};
	if( inet_pton(AF_INET, host, &addr.sin_addr) == -1)  {
		return -1;
	}

	if( connect(fd, (struct sockaddr*)&addr, sizeof(addr)) == -1) {
		return -1;
	}

	return fd;
}



int inet_close(int fd) {
	return close(fd);
}




int inet_set_read_timeout(int fd, unsigned timeout_sec) {
	struct timeval tv = {
		.tv_sec = timeout_sec,
		.tv_usec = 0
	};

	return setsockopt(fd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));
}



char *inet_address_tostring(const struct sockaddr *addr, socklen_t addrlen, char *buffer, size_t *len) {
	inet_ntoa_r(addr, buffer, *len);

	unsigned port = addr->sa_family == AF_INET
		? ((struct  sockaddr_in  *) addr)->sin_port
		: ((struct  sockaddr_in6 *) addr)->sin6_port;

	*len = snprintf(buffer + strlen(buffer), *len, ":%d", ntohs(port));
	
	return buffer;
}

// TODO refine error handling
char *inet_socket_tostring(int socket, int self, char *buffer, size_t *len) {
	struct sockaddr_storage _addr;
	struct sockaddr* addr = (struct sockaddr*) &_addr;
	socklen_t socklen = sizeof(_addr);

	int (*getname)(int, struct sockaddr *, socklen_t*);
	getname = self ? getsockname : getpeername;
	if( -1 == getname(socket, addr, &socklen) )
		goto error;

	if( NULL == inet_address_tostring(addr, socklen, buffer, len) )
		goto error;

	return buffer;


	error:
		return NULL;
}

char *inet_peer_socket_tostring(int socket, char *buffer, size_t *len) {
	return inet_socket_tostring(socket, false, buffer, len);
}
char *inet_self_socket_tostring(int socket, char *buffer, size_t *len) {
	return inet_socket_tostring(socket, true, buffer, len);
}




int inet_listen(const char *service, int backlog, socklen_t *addrlen) {
	fprintf(stderr, "inet_listen() not implemented\n");
	exit(EXIT_FAILURE);
}
int inet_bind(const char *service, int type, socklen_t *addrlen) {
	fprintf(stderr, "inet_listen() not implemented\n");
	exit(EXIT_FAILURE);
}



// static
// int _passive_socket(const char *service, int type, socklen_t *addrlen, bool do_listen, int backlog) {
// 	int sfd = 0;

// 	struct addrinfo hints = {
// 		.ai_canonname = NULL,
// 		.ai_addr = NULL,
// 		.ai_next = NULL,
// 		.ai_socktype = type,
// 		.ai_family = AF_UNSPEC,
// 		.ai_flags = AI_PASSIVE
// 	};
	
// 	if(_is_all_number(service)) {
// 		hints.ai_flags |= AI_NUMERICSERV;
// 	}


// 	struct addrinfo *addresses;
// 	if( getaddrinfo(NULL, service, &hints, &addresses) )
// 		return -1;

// 	struct addrinfo *addr = addresses;
// 	while(addr) {
// 		sfd = socket(addr->ai_family, addr->ai_socktype, addr->ai_protocol);
// 		if( sfd == -1 )
// 			continue;
		
// 		if(do_listen) {
// 			int optval = 1;
// 			if(setsockopt(sfd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)) == -1) {
// 				close(sfd);
// 				freeaddrinfo(addresses);
// 				return -1;
// 			}
// 		}

// 		if( bind(sfd, addr->ai_addr, addr->ai_addrlen) == 0)
// 			break; // success
		
// 		close(sfd);
// 		addr = addr->ai_next;
// 	}

// 	if(addr != NULL && do_listen) {
// 		if(listen(sfd, backlog) == -1) {
// 			freeaddrinfo(addresses);
// 			return -1;
// 		}
// 	}

// 	if(addr != NULL && addrlen != NULL)
// 		*addrlen = addr->ai_addrlen;

// 	freeaddrinfo(addresses);
// 	return (addr == NULL) ? -1 : sfd;
// }


// int inet_listen(const char *service, int backlog, socklen_t *addrlen) {
// 	bool do_listen = true;
// 	return _passive_socket(service, SOCK_STREAM, addrlen, do_listen, backlog);
// }
// int inet_bind(const char *service, int backlog, socklen_t *addrlen) {
// 	bool do_listen = false;
// 	return _passive_socket(service, SOCK_STREAM, addrlen, do_listen, backlog);
// }


// TODO refine error handling
char *inet_socket_tostring(int socket, int self, char *buffer, size_t *len) {
	struct sockaddr_storage _addr;
	struct sockaddr* addr = (struct sockaddr*) &_addr;
	socklen_t socklen = sizeof(_addr);

	int (*getname)(int, struct sockaddr *, socklen_t*);
	getname = self ? getsockname : getpeername;
	if( -1 == getname(socket, addr, &socklen) )
		goto error;

	if( NULL == inet_address_tostring(addr, socklen, buffer, len) )
		goto error;

	return buffer;


	error:
		return NULL;
}

char *inet_peer_socket_tostring(int socket, char *buffer, size_t *len) {
	return inet_socket_tostring(socket, false, buffer, len);
}
char *inet_self_socket_tostring(int socket, char *buffer, size_t *len) {
	return inet_socket_tostring(socket, true, buffer, len);
}

char *inet_address_tostring(const struct sockaddr *addr, socklen_t addrlen, char *buffer, size_t *len) {
	// char host[NI_MAXHOST];
	// char service[NI_MAXSERV];
	char host[1024];
	char service[1024];
	int e = getnameinfo(
		addr,    addrlen,
		host,    1024,
		service, 1024,
		NI_NUMERICSERV | NI_NUMERICHOST
	);

	if(e) return NULL;

	*len = snprintf(buffer, *len, "%s:%s", host, service);
	return buffer;
}