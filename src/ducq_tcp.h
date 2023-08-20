#ifndef _DUCQ_TCP_
#define _DUCQ_TCP_

#include "ducq.h"


ducq_i *ducq_new_tcp(const char *host, const char *port);
ducq_i *ducq_new_tcp_connection(int fd);

ducq_state ducq_tcp_apply(int cfd, ducq_apply_f apply, void* ctx);


#endif // _DUCQ_TCP_
