#ifndef _DUCQ_SRV_INT_HEADER_
#define _DUCQ_SRV_INT_HEADER_

#include <stddef.h>

#include "ducq.h"



typedef struct ducq_sub ducq_sub;
typedef struct ducq_srv ducq_srv;
typedef struct ducq_cmd ducq_cmd;


typedef struct ducq_sub {
	const char   *id;
	char   *route;
	ducq_t *ducq;
	ducq_sub  *next;
} ducq_sub;



typedef ducq_state (*command_f)(ducq_srv*, ducq_t *, char *, size_t);

struct ducq_cmd_t {
	char *name;
	char *doc;
	command_f exec;
};

struct ducq_cmd {
	struct ducq_cmd_t *cmd;
	void *handle;
};


struct ducq_srv {
	ducq_sub *subs;
	struct ducq_cmd *cmds;
	int ncmd;
};





#endif // _DUCQ_SRV_INT_HEADER_