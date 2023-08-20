#include <string.h>

#include <errno.h>

#include "inet_http.h"


ssize_t inet_get_http_header(int fd, char *buffer, size_t size, char **end) {
	char *start = buffer;
	char *max   = buffer + size;
	*end        = NULL;	

	while(buffer < max) {
		ssize_t n = read(fd, buffer, max-buffer);

		if(n <= 0) {
			     if(    n == 0          ) break;
			else if(errno == EWOULDBLOCK) break;
			else if(errno == EINTR      ) continue;
			else return -1;
		}
	
		buffer += n;
		if( *end = strstr(start, "\r\n\r\n") )
			break;
	}

	return buffer - start;
}



