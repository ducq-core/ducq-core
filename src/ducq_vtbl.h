#ifndef _DUCQ_VTABLE_HEADER_
#define _DUCQ_VTABLE_HEADER_

#include <stddef.h>
#include <stdbool.h>

// #include "ducq_def.h"


typedef enum ducq_state ducq_state;
typedef struct ducq_i ducq_i;

typedef struct ducq_vtbl {
	ducq_state   (* conn)    (ducq_i *ducq);
	ducq_state   (* send)    (ducq_i *ducq, void *ptr, size_t *size);
	ducq_state   (* recv)    (ducq_i *ducq, char *ptr, size_t *size);
	const char * (* id)      (ducq_i *ducq);
	ducq_i *     (* copy)    (ducq_i *ducq);
	bool         (* eq)      (ducq_i *a, ducq_i *b);
	ducq_state   (* timeout) (ducq_i *ducq, int timeout);
	ducq_state   (* close)   (ducq_i *ducq);
	void         (* free)    (ducq_i *ducq);
} ducq_vtbl;



#endif // _DUCQ_VTABLE_HEADER_