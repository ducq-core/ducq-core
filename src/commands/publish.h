#ifndef _DUCQ_PUBLISH_HEADER_
#define _DUCQ_PUBLISH_HEADER_

#include "../ducq_srv_int.h"

ducq_state publish(struct ducq_srv *srv, ducq_i *ducq, char *buffer, size_t size);

#endif // _DUCQ_PUBLISH_HEADER_