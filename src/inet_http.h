#ifndef __INET_HTTP_HEADER__
#define __INET_HTTP_HEADER__

#include <stddef.h>  //  size_t
#include <unistd.h>  // ssize_t


#define HTTP_SWITCHING_PROTOCOLS "HTTP/1.1 101 Switching Protocols"
#define HTTP_BAD_REQUEST_400     "HTTP/1.1 400 Bad Request\r\n\r\n"
ssize_t inet_get_http_header(int fd, char *buffer, size_t size, char **end);

char *inet_find_http_header(char *http_header, char *header, char **end);

#endif // __INET_HTTP_HEADER__
