#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>

#include "inet_socket.h"
#include "inet_http.h"
#include "inet_ws.h"

#include "ducq.h"
#include "ducq_vtbl.h"
#include "ducq_http.h"
#include "ducq_ws.h"




#define  MAX_ID 99 // 5 + 46 + 6 + 1; // HTTP:ipv6(46):65535\0

#ifndef DUCQ_HTTP_TIMEOUT
#define DUCQ_HTTP_TIMEOUT 5
#endif

typedef struct ducq_http {
	ducq_vtbl *tbl;
	
	int   fd;
	char  id[MAX_ID];
	
	const char *host;
	const char *port;

	char header[1024];
	ducq_i *ws;
} ducq_http;





static
ducq_state _conn(ducq_i *ducq) {
	return DUCQ_ENOIMPL;
}

static
const char *_id(ducq_i *ducq) {
	ducq_http *http = (ducq_http*)ducq;
	if(http->ws) 
		return ducq_id(http->ws);

	if(http->id[0] != '\0') return http->id;

	int n = snprintf(http->id, MAX_ID, "HTTP:");
	inet_peer_socket_tostring(http->fd, http->id + n, MAX_ID - n);
	return http->id;
}

static
ducq_state _timeout(ducq_i *ducq, int timeout) {
	ducq_http *http = (ducq_http*)ducq;
	if(http->ws) 
		return ducq_timeout(http->ws, timeout);

	int rc = inet_set_read_timeout(http->fd, timeout);
	return rc ? DUCQ_ECOMMLAYER : DUCQ_OK;
}

static
ducq_state _recv(ducq_i *ducq, char *ptr, size_t *count) {
	ducq_http *http = (ducq_http*)ducq;
	if(http->ws) 
		return ducq_recv(http->ws, ptr, count);

	char *end = NULL;
	ssize_t n = 0;

	n = inet_get_http_header(http->fd, http->header, sizeof(http->header), &end);
	if(!end) {
		writen(http->fd, HTTP_BAD_REQUEST_400, sizeof(HTTP_BAD_REQUEST_400)-1); 
		return DUCQ_EMSGINV;
	}
	*end = '\0';


	if( strstr(http->header, "\r\nUpgrade; websocket\r\n") == 0 )
		return ducq_new_ws_upgrade_from_http(&http->ws, http->fd, http->header);


	return DUCQ_ENOIMPL;
}

static
ducq_state _send(ducq_i *ducq, const void *buf, size_t *count) {
	ducq_http *http = (ducq_http*) ducq;
	if(http->ws) 
		return ducq_send(http->ws, buf, count);

	return DUCQ_ENOIMPL;
}

static
ducq_i *_copy(ducq_i * ducq) {
	ducq_http *http = (ducq_http*)ducq;
	if(http->ws) 
		return ducq_copy(http->ws);

	ducq_http *copy = (ducq_http*) ducq_new_http_client(http->host, http->port);
	copy->fd = http->fd;
	return (ducq_i*) copy;
}

static
bool _eq(ducq_i *a, ducq_i *b) {
	ducq_http *http = (ducq_http*)a;
	if(http->ws) 
		return ducq_eq(http->ws, b);

	if(b == NULL) return false;

	ducq_http *oth = (ducq_http*)b;

	return http->tbl == oth->tbl
	    && http->fd  == oth->fd;
}

static
ducq_state _close(ducq_i *ducq) {
	ducq_http *http = (ducq_http*)ducq;
	if(http->ws) 
		return ducq_close(http->ws);

	int rc = inet_close(http->fd);
	if( rc == -1)
		return DUCQ_ECLOSE;
	return DUCQ_OK;
}

static
void _free (ducq_i *ducq) {
	ducq_http *http = (ducq_http*)ducq;
	if(!http) return;
	if(http->ws) 
		ducq_free(http->ws);
	free(ducq);
}

static
void _dtor (ducq_i *ducq) {
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

ducq_i *ducq_new_http_client(const char *host, const char *port) {
	ducq_http *http = malloc(sizeof(ducq_http));
	if(!http) return NULL;

	http->tbl   = &table;
	http->fd    = -1;
	http->host  = host;
	http->port  = port;
	http->id[0] = '\0';
	http->ws    = NULL;

	return (ducq_i *) http;
}



ducq_i *ducq_new_http_connection(int fd) {
	ducq_http *http = malloc(sizeof(ducq_http));
	if(!http) return NULL;

	http->tbl   = &table;
	http->fd    = fd;
	http->host  = NULL;
	http->port  = NULL;
	http->id[0] = '\0';
	http->ws    = NULL;

	inet_set_read_timeout(fd, DUCQ_HTTP_TIMEOUT);
	
	return (ducq_i *) http;
}



