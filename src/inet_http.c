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
		if(( *end = strstr(start, "\r\n\r\n") )) {
			*end += 2;
			break;
		}
	}

	return buffer - start;
}


// expects `http_header` to be null terminated.
char *inet_find_http_header(char *http_header, char *header, char **end) {
	char *found = http_header;
	while(( found = strstr(found, header) )) {
		found += strlen(header);
		if(*found != ':') continue;

		while(*(++found) == ' ')

		*end = strstr(found, "\r\n");
		break;
	}

	return *end ? found : NULL;
}

