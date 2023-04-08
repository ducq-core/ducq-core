#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>

// #include <unistd.h>

#include "inet_socket.h"

#include "ducq_def.h"
#include "ducq.h"
#include "ducq_i.h"

#include "ducq_tcp.h"

#define  MAX_ID 100 // 4 + 46 + 6 + 1; // TCP:ipv6(46):65535\0

typedef struct ducq_tcp_t {
	ducq_i *tbl;
	
	int fd;
	const char *host;
	const char *port;
	char  id[MAX_ID];
} ducq_tcp_t;



int ducq_tcp_recv(int fd, char *ptr, size_t size) {	
	int n = 0;
	char *start = ptr;
	*ptr = '\0';
	
	while (n = readn(fd, ptr, 1) > 0){
		if(!isdigit(*ptr))
			break;
		ptr++;
	}

	if(n <  0)        return -DUCQ_EREAD;
	if(n == 0)        return -DUCQ_ECONNCLOSED;
	if(ptr == start)  return -DUCQ_EMSGINV;
	if(*ptr != '\n')  return -DUCQ_EMSGINV;
	
	*ptr = '\0';
	int len = atoi(start);
	if( len+1 > size) return -DUCQ_EMSGSIZE;

	n = readn(fd, start, len);
	if(n <= 0)        return  -DUCQ_EREAD;
	if(n != len)      return  -DUCQ_EREAD;
	
	start[len] = '\0';
	return n;
}
int ducq_tcp_send(int fd, void *buf, size_t count) {
	char header[10];
	int len = snprintf(header, 10, "%ld\n", count);

	writen(fd, header, len);
	if(writen(fd, buf, count) != count)
		return -DUCQ_EWRITE;

	return count;
}



static
ducq_state _conn(ducq_t *ducq) {
	ducq_tcp_t *tcp = (ducq_tcp_t*)ducq;

	if(tcp->fd > 0)
		inet_close(tcp->fd);

	int fd = inet_tcp_connect(tcp->host, tcp->port);
	if( fd == -1)
		return DUCQ_ECONNECT;
	
	tcp->fd = fd;
	return DUCQ_OK;
}

static
const char *_id(ducq_t *ducq) {
	ducq_tcp_t *tcp = (ducq_tcp_t*)ducq;

	if(tcp->id[0] != '\0') return tcp->id;

	size_t size = MAX_ID;
	return inet_peer_socket_tostring(tcp->fd, tcp->id, &size);
}
static
ducq_state _timeout(ducq_t *ducq, int timeout) {
	ducq_tcp_t *tcp = (ducq_tcp_t*)ducq;

	int rc = inet_set_read_timeout(tcp->fd, timeout);
	return rc ? DUCQ_ECOMMLAYER : DUCQ_OK;
}


static
ducq_state _recv(ducq_t *ducq, char *ptr, size_t *count) {
	ducq_tcp_t *tcp = (ducq_tcp_t*)ducq;

	int rc = ducq_tcp_recv(tcp->fd, ptr, *count);
	if( rc < 0)
		return -rc;
	
	*count = rc;
	return DUCQ_OK;
}


static
ducq_state _send(ducq_t *ducq, void *buf, size_t *count) {
	ducq_tcp_t *tcp = (ducq_tcp_t*) ducq;

	int rc = ducq_tcp_send(tcp->fd, buf, *count);
	if( rc < 0)
		return -rc;
	
	*count = rc;
	return DUCQ_OK;	
}

static
ducq_t *_copy(ducq_t * ducq) {
	ducq_tcp_t *tcp = (ducq_tcp_t*)ducq;
	return ducq_new_tcp(tcp->fd, tcp->host, tcp->port);
}
static
bool _eq(ducq_t *a, ducq_t *b) {
	ducq_tcp_t *tcp = (ducq_tcp_t*)a;
	ducq_tcp_t *oth = (ducq_tcp_t*)b;

	return tcp->tbl == oth->tbl
	    && tcp->fd  == oth->fd;
}
static
ducq_state _close(ducq_t *ducq) {
	ducq_tcp_t *tcp = (ducq_tcp_t*)ducq;

	int rc = inet_close(tcp->fd);
	if( rc == -1)
		return DUCQ_ECLOSE;
	return DUCQ_OK;
}

static
void _free (ducq_t *ducq) {
	if(ducq) free(ducq);
}




static ducq_i table = {
	.conn    = _conn,
	.close   = _close,
	.id      = _id,
	.recv    = _recv,
	.send    = _send,
	.copy    = _copy,
	.eq      = _eq,
	.timeout = _timeout,
	.free    = _free
};

ducq_t *ducq_new_tcp(int fd, const char *host, const char *port) {
	ducq_tcp_t *tcp = malloc(sizeof(ducq_tcp_t));
	if(!tcp) return NULL;

	tcp->tbl = &table;
	tcp->fd   = fd;
	tcp->host = host;
	tcp->port = port;
	tcp->id[0] = '\0';

	return (ducq_t *) tcp;
}





ducq_state ducq_tcp_apply(int cfd, ducq_apply_f apply, void* cl) {
	ducq_tcp_t ducq = { .tbl = &table, .fd = cfd	};
	return apply(cl, (ducq_t*) &ducq);
}
