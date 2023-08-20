#ifndef __INET_HTTP_HEADER__
#define __INET_HTTP_HEADER__

#include <stddef.h>  //  size_t
#include <unistd.h>  // ssize_t



ssize_t inet_get_http_header(int fd, char *buffer, size_t size, char **end);



#endif // __INET_HTTP_HEADER__
