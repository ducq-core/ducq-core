#ifndef _DUCQ_HEADER_
#define _DUCQ_HEADER_

#include <stdbool.h>


#ifndef DUCQ_MSGSZ
#define DUCQ_MSGSZ 256
#endif


#define _foreach_state(apply) \
	apply(DUCQ_ESTDC,       "std c lib error") \
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
	apply(DUCQ_ENOCMD,      "command unknown") \
	apply(DUCQ_EACK,        "no ack received") \


typedef enum ducq_state {
	DUCQ_OK = 0,
	#define list_constants(s, _) s,
	_foreach_state(list_constants)
	#undef list_constants
} ducq_state;

const char *ducq_state_tostr(int state);



typedef struct ducq_i ducq_i;

ducq_state  ducq_conn(ducq_i *ducq);
ducq_state  ducq_send(ducq_i *ducq, const void *buf, size_t *size);
ducq_state  ducq_recv(ducq_i *ducq, void *buf, size_t *size);
ducq_state  ducq_emit(ducq_i *ducq, const char *command, const char *route, const char *payload, size_t payload_size, bool close);
const char *ducq_id  (ducq_i *ducq);
bool        ducq_eq  (ducq_i *a, ducq_i *b);
ducq_i     *ducq_copy(ducq_i *ducq);
ducq_state  ducq_timeout(ducq_i *ducq, int timeout);
ducq_state  ducq_close(ducq_i *ducq);
void        ducq_free (ducq_i *ducq);


typedef ducq_state (*ducq_apply_f)(void* ctx, ducq_i* ducq);

ducq_state ducq_publish(ducq_i *ducq, char *route, char *payload, size_t size);


const char * ducq_parse_command(const char *buffer, const char **end);
const char * ducq_parse_route(const char *buffer, const char **end);
const char * ducq_parse_payload(const char *buffer);
ducq_state ducq_ack_to_state(const char *msg);

bool ducq_route_cmp(const char *sub_route, const char *pub_route);


#endif // _DUCQ_HEADER_