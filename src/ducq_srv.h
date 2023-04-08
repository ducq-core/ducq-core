#ifndef _DUCQ_SRV_HEADER_
#define _DUCQ_SRV_HEADER_

#include "ducq.h"

typedef struct ducq_srv ducq_srv;


ducq_srv *ducq_srv_new();
void ducq_srv_free(ducq_srv* srv);
ducq_state ducq_srv_dispatch(ducq_srv *srv, ducq_t *ducq);

int ducq_srv_unsub(ducq_srv *srv, ducq_t *ducq);


#endif // _DUCQ_SRV_HEADER_