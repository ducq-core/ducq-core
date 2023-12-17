#ifndef _DUCQ_HEADER_
#define _DUCQ_HEADER_

#include <stddef.h>
#include <stdbool.h>



#ifndef DUCQ_MSGSZ
#define DUCQ_MSGSZ 256
#endif


#define _foreach_state(apply) \
	apply(DUCQ_PROTOCOL,    "protocol-layer message (not an error)") \
	apply(DUCQ_ENULL,       "null parameter") \
	apply(DUCQ_ESTDC,       "std c lib error") \
	apply(DUCQ_EMEMFAIL,    "os memory allocation failed") \
	apply(DUCQ_EFILE,       "os file io error") \
	apply(DUCQ_ECOMMLAYER,  "os communication layer error") \
	apply(DUCQ_ECONNECT,    "could not connect to server") \
	apply(DUCQ_ECONNCLOSED, "connection closed") \
	apply(DUCQ_EWRITE,      "communication write error") \
	apply(DUCQ_EREAD,       "communication read error") \
	apply(DUCQ_ECLOSE,      "error closing connection") \
	apply(DUCQ_ETIMEOUT,    "timeout") \
	apply(DUCQ_EMSGINV,     "message invalid") \
	apply(DUCQ_EMSGSIZE,    "message too big") \
	apply(DUCQ_ENOCMD,      "command unknown") \
	apply(DUCQ_EACK,        "no ack received") \
	apply(DUCQ_ENOTFOUND,	"connection not found") \
	apply(DUCQ_ENOIMPL,	"not implemented yet") \
	apply(DUCQ_EMAX,	"collection reached maximum count") \
	apply(DUCQ_ELUA,	"lua error")

typedef enum ducq_state {
	DUCQ_OK = 0,
	#define list_constants(s, _) s,
	_foreach_state(list_constants)
	#undef list_constants
} ducq_state;

const char *ducq_state_tostr(int state);



#define DUCQ_CHECK(func) do { ducq_state state = func; if(state != DUCQ_OK) return state; } while(0)


// interface
typedef struct ducq_i ducq_i;

ducq_state  ducq_conn(ducq_i *ducq);
ducq_state  ducq_send(ducq_i *ducq, const void *buf, size_t *size);
ducq_state  ducq_parts(ducq_i *ducq);
ducq_state  ducq_end(ducq_i *ducq);
ducq_state  ducq_recv(ducq_i *ducq, char *buf, size_t *size);
const char *ducq_id  (ducq_i *ducq);
bool        ducq_eq  (ducq_i *a, ducq_i *b);
ducq_i     *ducq_copy(ducq_i *ducq);
ducq_state  ducq_timeout(ducq_i *ducq, int timeout);
ducq_state  ducq_close(ducq_i *ducq);
void        ducq_free (ducq_i *ducq);


// higher level
typedef ducq_state (*ducq_apply_f)(void* ctx, ducq_i* ducq);
typedef int (*ducq_on_msg_f)(ducq_i * ducq, char *payload, size_t size, void *ctx);
struct ducq_listen_ctx {
	ducq_on_msg_f on_message;
	ducq_on_msg_f on_protocol;
	ducq_on_msg_f on_error;
	bool recv_raw;
	void *ctx;
};

ducq_state  ducq_emit(ducq_i *ducq, const char *command, const char *route, const char *payload, size_t payload_size);
ducq_state ducq_send_ack(ducq_i *ducq, ducq_state state);
ducq_state  ducq_receive(ducq_i *ducq, char *msg, size_t *size);
ducq_state ducq_listen(ducq_i *ducq, struct ducq_listen_ctx *ctx);
//ducq_state ducq_subscribe(ducq_i *ducq, const char *route, ducq_on_msg_f on_msg, void *ctx);
ducq_state ducq_publish(ducq_i *ducq, const char *route, const char *payload, size_t size);



// message parsing
// none-destructive
const char * ducq_parse_command(const char *buffer, const char **end);
const char * ducq_parse_route(const char *buffer, const char **end);
const char * ducq_parse_payload(const char *buffer);
ducq_state ducq_ack_to_state(const char *msg);

bool ducq_route_cmp(const char *sub_route, const char *pub_route);

// desctructive
struct ducq_msg {
	char *command;
	char *route;
	char *payload;
};
struct ducq_msg ducq_parse_msg(char *message);


#endif // _DUCQ_HEADER_
