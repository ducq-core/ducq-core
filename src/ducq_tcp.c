#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>

#include "ducq.h"
#include "ducq_vtbl.h"
#include "inet_socket.h"

#include "ducq_tcp.h"


#define  MAX_ID 100 // 4 + 46 + 6 + 1; // TCP:ipv6(46):65535\0

typedef struct ducq_tcp_t {
	ducq_vtbl *tbl;
	
	int fd;
	const char *host;
	const char *port;
	char  id[MAX_ID];
} ducq_tcp_t;



int ducq_tcp_recv(int fd, char *ptr, size_t size) {	
	ssize_t n = 0;
	char *start = ptr;
	char *end   = start + size - 1;
	
	while ( (n = readn(fd, ptr, 1)) > 0){
		if(!isdigit((int)*ptr))
			break;

		ptr++;
		if(ptr >= end)
			return -DUCQ_EMSGSIZE;
	}

	if(n <  0)        return -DUCQ_EREAD;
	if(n == 0)        return -DUCQ_ECONNCLOSED;
	if(*ptr != '\n')  return -DUCQ_EMSGINV;
	
	*ptr = '\0';
	int len = atoi(start);
	if( len+1 > size) return -DUCQ_EMSGSIZE;

	n = readn(fd, start, len);
	if(n <  0)        return -DUCQ_EREAD;
	if(n == 0)        return -DUCQ_ECONNCLOSED;
	if(n != len)      return -DUCQ_EREAD;
	
	start[len] = '\0';
	return n;
}

int ducq_tcp_send(int fd, const void *buf, size_t count) {
	#define MAX_SIZE_T_LENGTH 25
	char header[MAX_SIZE_T_LENGTH];
	int len = snprintf(header, MAX_SIZE_T_LENGTH, "%ld\n", count);

	if(len < 0 || len >= MAX_SIZE_T_LENGTH)
		return -DUCQ_ESTDC;

	if(writen(fd, header, len) != len)
		return -DUCQ_EWRITE;
	if(writen(fd, buf, count) != count)
		return -DUCQ_EWRITE;

	return count;
}



static
ducq_state _conn(ducq_i *ducq) {
	// printf("in %s\n", __func__);
	ducq_tcp_t *tcp = (ducq_tcp_t*)ducq;

	int fd = inet_tcp_connect(tcp->host, tcp->port);
	if( fd == -1)
		return DUCQ_ECONNECT;
	
	tcp->fd = fd;
	return DUCQ_OK;
}


static
const char *_id(ducq_i *ducq) {
	// printf("in %s\n", __func__);
	ducq_tcp_t *tcp = (ducq_tcp_t*)ducq;

	if(tcp->id[0] != '\0') return tcp->id;

	int n = snprintf(tcp->id, MAX_ID, "TCP:");
	inet_peer_socket_tostring(tcp->fd, tcp->id + n, MAX_ID - n);
	return tcp->id;
}
static
ducq_state _timeout(ducq_i *ducq, int timeout) {
	// printf("in %s\n", __func__);
	ducq_tcp_t *tcp = (ducq_tcp_t*)ducq;

	int rc = inet_set_read_timeout(tcp->fd, timeout);
	return rc ? DUCQ_ECOMMLAYER : DUCQ_OK;
}


static
ducq_state _recv(ducq_i *ducq, char *ptr, size_t *count) {
	// printf("in %s\n", __func__);
	ducq_tcp_t *tcp = (ducq_tcp_t*)ducq;

	int rc = ducq_tcp_recv(tcp->fd, ptr, *count);
	if( rc < 0)
		return -rc;
	
	*count = rc;
	return DUCQ_OK;
}


static
ducq_state _send(ducq_i *ducq, const void *buf, size_t *count) {
	// printf("in %s\n", __func__);
	ducq_tcp_t *tcp = (ducq_tcp_t*) ducq;

	int rc = ducq_tcp_send(tcp->fd, buf, *count);
	if( rc < 0)
		return -rc;
	
	*count = rc;
	return DUCQ_OK;	
}

static
ducq_i *_copy(ducq_i * ducq) {
	// printf("in %s\n", __func__);
	ducq_tcp_t *tcp = (ducq_tcp_t*)ducq;
	return ducq_new_tcp(tcp->fd, tcp->host, tcp->port);
}

static
bool _eq(ducq_i *a, ducq_i *b) {
	// printf("in %s\n", __func__);
	ducq_tcp_t *tcp = (ducq_tcp_t*)a;
	ducq_tcp_t *oth = (ducq_tcp_t*)b;

	return tcp->tbl == oth->tbl
	    && tcp->fd  == oth->fd;
}

static
ducq_state _close(ducq_i *ducq) {
	// printf("in %s -- %p\n", __func__, ducq);
	ducq_tcp_t *tcp = (ducq_tcp_t*)ducq;

	int rc = inet_close(tcp->fd);
	if( rc == -1)
		return DUCQ_ECLOSE;
	return DUCQ_OK;
}

static
void _free (ducq_i *ducq) {
	// printf("in %s\n", __func__);
	if(ducq) free(ducq);
}

static
void _dtor (ducq_i *ducq) {
	// printf("in %s\n", __func__);
}




static ducq_vtbl table = {
	.conn    = _conn,
	.close   = _close,
	.id      = _id,
	.recv    = _recv,
	.send    = _send,
	.copy    = _copy,
	.eq      = _eq,
	.timeout = _timeout,
	.free    = _free,
	.dtor    = _dtor
};

ducq_i *ducq_new_tcp(int fd, const char *host, const char *port) {
	ducq_tcp_t *tcp = malloc(sizeof(ducq_tcp_t));
	if(!tcp) return NULL;

	tcp->tbl = &table;
	tcp->fd   = fd;
	tcp->host = host;
	tcp->port = port;
	tcp->id[0] = '\0';

	return (ducq_i *) tcp;
}





ducq_state ducq_tcp_apply(int cfd, ducq_apply_f apply, void* ctx) {
	ducq_tcp_t ducq = { .tbl = &table, .fd = cfd };
	return apply(ctx, (ducq_i*) &ducq);
}
