#ifndef _DUCQ_HEADER_
#define _DUCQ_HEADER_

#include <stdbool.h>



#define _foreach_state(apply) \
	apply(DUCQ_OK,          "ok") \
	apply(DUCQ_EMEMFAIL,    "os memory allocation failed") \
	apply(DUCQ_EFILE,       "os file io error") \
	apply(DUCQ_ECOMMLAYER,  "os communication layer error") \
	apply(DUCQ_ECONNECT,    "could not connect to server") \
	apply(DUCQ_ECONNCLOSED, "connection closed") \
	apply(DUCQ_EWRITE,      "communication write error") \
	apply(DUCQ_EREAD,       "communication read error") \
	apply(DUCQ_ECLOSE,      "error closing connection") \
	apply(DUCQ_EMSGINV,     "message invalid") \
	apply(DUCQ_EMSGSIZE,    "message too big") \
	apply(DUCQ_EACK,        "no ack received") \


typedef enum ducq_state {
	#define list_constants(s, _) s,
	_foreach_state(list_constants)
	#undef list_constants
} ducq_state;

const char *ducq_state_tostr(int state);



typedef struct ducq_i ducq_i;

ducq_state  ducq_conn(ducq_i *ducq);
ducq_state  ducq_send(ducq_i *ducq, void *buf, size_t *size);
ducq_state  ducq_recv(ducq_i *ducq, void *buf, size_t *size);
const char *ducq_id  (ducq_i *ducq);
bool        ducq_eq  (ducq_i *a, ducq_i *b);
ducq_i     *ducq_copy(ducq_i *ducq);
ducq_state  ducq_timeout(ducq_i *ducq, int timeout);
ducq_state  ducq_close(ducq_i *ducq);
void        ducq_free (ducq_i *ducq);



#endif // _DUCQ_HEADER_