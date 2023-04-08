#ifndef _DUCQ_INTERFACE_HEADER_
#define _DUCQ_INTERFACE_HEADER_

#include <stddef.h>
#include <stdbool.h>

// #include "ducq_def.h"


typedef enum ducq_state ducq_state;
typedef struct ducq_t ducq_t;

typedef struct ducq_i {
	ducq_state   (* conn)    (ducq_t *ducq);
	ducq_state   (* send)    (ducq_t *ducq, void *ptr, size_t *size);
	ducq_state   (* recv)    (ducq_t *ducq, char *ptr, size_t *size);
	const char * (* id)      (ducq_t *ducq);
	ducq_t *     (* copy)    (ducq_t *ducq);
	bool         (* eq)      (ducq_t *a, ducq_t *b);
	ducq_state   (* timeout) (ducq_t *ducq, int timeout);
	ducq_state   (* close)   (ducq_t *ducq);
	void         (* free)    (ducq_t *ducq);
} ducq_i;



#endif // _DUCQ_INTERFACE_HEADER_