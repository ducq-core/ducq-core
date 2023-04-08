#ifndef _DUCQ_DEF_HEADER_
#define _DUCQ_DEF_HEADER_


// could be in ducq.h


#define DUCQ_PUBLISH   'P'
#define DUCQ_SUBSCRIBE 'S'
#define DUCQ_ACK       'A'
#define DUCQ_NACK      'N'


#ifndef DUCQ_MAX_MSG_LEN
#define DUCQ_MAX_MSG_LEN 512
#endif


typedef enum {
	DUCQ_OK,

// os
	DUCQ_EMEMFAIL,
	DUCQ_EFILE,
	DUCQ_ECOMMLAYER,

// communication
	DUCQ_ECONNECT,
	DUCQ_ECONNCLOSED,
	DUCQ_EWRITE,
	DUCQ_EREAD,
	DUCQ_ECLOSE,

// protocol
	DUCQ_EMSGINV,
	DUCQ_EMSGSIZE,
	DUCQ_EACK,

	DUCQ_COUNT
} ducq_state;


// typedef struct ducq_ep_i ducq_ep_i;
typedef struct ducq_t ducq_t;


#endif // _DUCQ_DEF_HEADER_