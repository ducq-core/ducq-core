#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>

#include "ducq.h"
#include "ducq_vtbl.h"
#include "ducq_ws.h"

#include "inet_socket.h"
#include "inet_http.h"
#include "inet_ws.h"

#define  MAX_ID 99 // 3 + 46 + 6 + 1; // WS:ipv6(46):65535\0

typedef struct ducq_ws {
	ducq_vtbl *tbl;
	
	int fd;
	char  id[MAX_ID];
	
	ws_header_t hdr;
	bool is_client;

	const char *host;
	const char *port;
} ducq_ws;




#include <assert.h>

static
ducq_state _conn(ducq_i *ducq) {
	return DUCQ_ENOIMPL;
}


static
const char *_id(ducq_i *ducq) {
	ducq_ws *ws = (ducq_ws*)ducq;

	if(ws->id[0] != '\0') return ws->id;

	int n = snprintf(ws->id, MAX_ID, "WS:");
	inet_peer_socket_tostring(ws->fd, ws->id + n, MAX_ID - n);
	return ws->id;
}
static
ducq_state _timeout(ducq_i *ducq, int timeout) {
	ducq_ws *ws = (ducq_ws*)ducq;

	int rc = inet_set_read_timeout(ws->fd, timeout);
	return rc ? DUCQ_ECOMMLAYER : DUCQ_OK;
}


static
ducq_state _recv(ducq_i *ducq, char *ptr, size_t *count) {
	ducq_ws *ws = (ducq_ws*)ducq;

	return DUCQ_ENOIMPL;
}


static
ducq_state _send(ducq_i *ducq, const void *buf, size_t *count) {
	ducq_ws *ws = (ducq_ws*) ducq;
	return DUCQ_ENOIMPL;
}

static
ducq_state _emit(ducq_i *ducq, const char *command, const char *route, const char *payload, size_t payload_size, bool close) {
	char msg[DUCQ_MSGSZ];
	size_t len = snprintf(msg, DUCQ_MSGSZ, "%s %s\n%.*s", command, route, (int)payload_size, payload);

	if(len >= DUCQ_MSGSZ)
		return DUCQ_EMSGSIZE;

	ducq_state state = ducq_send(ducq, msg, &len);
	if(state != DUCQ_OK) return state;

	if(close) {
		ducq_ws *ws = (ducq_ws*)ducq;
		if(inet_shutdown_write(ws->fd))
			return DUCQ_ECLOSE;
	}

	return DUCQ_OK;
}


static
ducq_i *_copy(ducq_i * ducq) {
	ducq_ws *ws = (ducq_ws*)ducq;
	return NULL;
}

static
bool _eq(ducq_i *a, ducq_i *b) {
	if(b == NULL) return false;

	ducq_ws *ws = (ducq_ws*)a;
	ducq_ws *oth = (ducq_ws*)b;

	return ws->tbl == oth->tbl
	    && ws->fd  == oth->fd;
}

static
ducq_state _close(ducq_i *ducq) {
	ducq_ws *ws = (ducq_ws*)ducq;

	int rc = inet_close(ws->fd);
	if( rc == -1)
		return DUCQ_ECLOSE;
	return DUCQ_OK;
}

static
void _free (ducq_i *ducq) {
	if(ducq) free(ducq);
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
	.emit    = _emit,
	.copy    = _copy,
	.eq      = _eq,
	.timeout = _timeout,
	.free    = _free,
	.dtor    = _dtor
};

ducq_i *ducq_new_ws(const char *host, const char *port) {
	ducq_ws *ws = malloc(sizeof(ducq_ws));
	if(!ws) return NULL;

	ws->tbl = &table;
	ws->fd   = -1;
	ws->host = host;
	ws->port = port;
	ws->id[0] = '\0';

	ws->is_client = true;
	memset(ws->hdr, 0, sizeof(ws_header_t));

	return (ducq_i *) ws;
}



ducq_i *ducq_new_ws_connection(int fd) {
	ducq_ws *ws = malloc(sizeof(ducq_ws));
	if(!ws) return NULL;

	ws->tbl   = &table;
	ws->fd    = fd;
	ws->host  = NULL;
	ws->port  = NULL;
	ws->id[0] = '\0';
	
	ws->is_client = false;
	memset(ws->hdr, 0, sizeof(ws_header_t));

	return (ducq_i *) ws;
}





ducq_state ducq_new_ws_upgrade_from_http(ducq_i **ws, int fd, char *http_header) {
	char *end        = NULL;
	char *client_key = inet_find_http_header(http_header, SEC_WEBSOCKET_KEY, &end);
	if(!client_key) {
		writen(fd, HTTP_BAD_REQUEST_400, sizeof(HTTP_BAD_REQUEST_400)-1); 
		return DUCQ_EMSGINV;
	}
	*end = '\0';

	char accept_key[WS_B64_ACCEPT_KEY_LEN] = "";
	ws_error_t error = ws_make_accept_key(client_key, accept_key, WS_B64_ACCEPT_KEY_LEN);
	if(error) assert(false && "ws_make_accept_key() failed"); // sha1 or base64 error
	
	char reply[DUCQ_MSGSZ] = "";
	ssize_t n = snprintf(reply, DUCQ_MSGSZ, 
                "HTTP/1.1 101 Switching Protocols\r\n"
                "Upgrade: websocket\r\n"
                "Connection: Upgrade\r\n"
                "Sec-WebSocket-Accept: %s\r\n"
                "\r\n", 
		accept_key);
	if(n < 0)           return DUCQ_ESTDC;
	if(n >= DUCQ_MSGSZ) return DUCQ_EMSGSIZE; 

	n = writen(fd, reply, n);
	if(n <= 0 ) return DUCQ_EWRITE;

	*ws = ducq_new_ws_connection(fd);
	if( !(*ws) ) return DUCQ_EMEMFAIL;

	return DUCQ_PROTOCOL;
}