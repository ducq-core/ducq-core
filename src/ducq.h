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




typedef struct ducq_t ducq_t;

ducq_state  ducq_conn(ducq_t *ducq);
ducq_state  ducq_send(ducq_t *ducq, void *buf, size_t *size);
ducq_state  ducq_recv(ducq_t *ducq, void *buf, size_t *size);
const char *ducq_id  (ducq_t *ducq);
bool        ducq_eq  (ducq_t *a, ducq_t *b);
ducq_t     *ducq_copy(ducq_t *ducq);
ducq_state  ducq_timeout(ducq_t *ducq, int timeout);
ducq_state  ducq_close(ducq_t *ducq);
void        ducq_free (ducq_t *ducq);



// ducq_state ducq_emit(ducq_t *ducq, const char *command, const char *route, const char *payload);

typedef ducq_state (*ducq_apply_f)(void *cl, ducq_t *ducq);


// sugar
typedef int (*on_msg_f)(const char *payload, size_t size, void *cl);
ducq_state ducq_subscribe(ducq_t *ducq, const char *route, on_msg_f onmsg, void *cl);
ducq_state ducq_publish  (ducq_t *ducq, const char *route, const char *payload);



#endif // _DUCQ_HEADER_