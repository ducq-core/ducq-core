#ifndef _DUCQ_SUBSCRIBE_HEADER_
#define _DUCQ_SUBSCRIBE_HEADER_

#include "../ducq_srv_int.h"

ducq_state subscribe(struct ducq_srv *srv, ducq_t *ducq, char *buffer, size_t size);

#endif // _DUCQ_SUBSCRIBE_HEADER_