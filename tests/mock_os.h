#ifndef _MOCK_OS_TESTS_HEADER_
#define _MOCK_OS_TESTS_HEADER_

#include <stddef.h>
#include <stdio.h>

#ifdef __linux__
	#include <sys/socket.h>  // sockaddr, socklen_t
#elif ESP_PLATFORM
	#include "lwip/sockets.h"
#endif


ssize_t mock_read(int fd, void *buf, size_t count);
ssize_t mock_write(int fd, const void *buf, size_t count);
int mock_getsockname(int fd, struct sockaddr *addr, socklen_t *len);

#endif // _MOCK_OS_TESTS_HEADER_