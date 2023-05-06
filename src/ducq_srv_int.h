#ifndef _DUCQ_SRV_INT_HEADER_
#define _DUCQ_SRV_INT_HEADER_

#include <stddef.h>

#include "ducq.h"



typedef struct ducq_sub ducq_sub;
typedef struct ducq_srv ducq_srv;
typedef struct ducq_cmd ducq_cmd;


typedef struct ducq_sub {
	const char *id;
	char       *route;
	ducq_i     *ducq;
	ducq_sub   *next;
} ducq_sub;



typedef ducq_state (*command_f)(ducq_srv*, ducq_i*, char *, size_t);

struct ducq_cmd_t {
	char *name;
	char *doc;
	command_f exec;
};



struct ducq_srv {
	ducq_sub *subs;
	// struct ducq_cmd *cmds;
	struct ducq_cmd_t **cmds;
	void **hdls;
	int ncmd;
};


ducq_state send_ack(ducq_i *ducq, ducq_state state);

bool ducq_srv_unsubscribe(ducq_srv *srv, ducq_i *ducq);
void ducq_sub_free(ducq_sub *sub);

#endif // _DUCQ_SRV_INT_HEADER_