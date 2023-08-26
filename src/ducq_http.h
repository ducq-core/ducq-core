#ifndef _DUCQ_HTTP_
#define _DUCQ_HTTP_

#include "ducq.h"


ducq_i *ducq_new_http_client(const char *host, const char *port);
ducq_i *ducq_new_http_connection(int fd);



#endif // _DUCQ_HTTP_
