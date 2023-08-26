#ifndef __WEBSOCKET_HEADER__
#define __WEBSOCKET_HEADER__

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>


// definitions
#define WS_GUID "258EAFA5-E914-47DA-95CA-C5AB0DC85B11"
#define SEC_WEBSOCKET_KEY "Sec-WebSocket-Key"

#define WS_FIN           0x80U
#define WS_RSV1          0x40U
#define WS_RSV2          0x20U
#define WS_RSV3          0x10U
#define WS_OPCODE        0x0FU

#define WS_MASK          0x80U
#define WS_LEN           0x7FU 

typedef enum {
        WS_CONTINUATION = 0,
        WS_TEXT,
        WS_BINARY,
        WS_CLOSE = 8,
        WS_PING,
        WS_PONG
} ws_opcode_t;

typedef enum {
	WS_CONNECTING,
	WS_OPEN,
	WS_FAILED,
	WS_CLOSING,
	WS_CLOSED
} ws_state_t;

typedef enum {
	WS_OK,
	WS_BASE64_NULL,
	WS_BASE64_OUT_LEN,
	WS_SHA1_NULL,
	WS_SHA1_IN_LEN,
	WS_SHA1_STATE,
	WS_NULL_PARAM,
	WS_IN_LEN,
	WS_OUT_LEN,

	WS_ERROR_COUNT
} ws_error_t;


// types
typedef union {
	uint32_t u32;
	unsigned char bytes[4];
} ws_mask_t;

typedef unsigned char byte_t;
typedef byte_t ws_header_t[ 2 + sizeof(uint64_t) + sizeof(ws_mask_t) ];
#define WS_HEADER_SIZE sizeof(ws_header_t)


// getters and setters
size_t     ws_get_hdr_len(ws_header_t hdr);
ws_mask_t *ws_get_msk(ws_header_t hdr);
void       ws_set_msk(ws_header_t hdr, void *mask);
uint64_t   ws_get_len(ws_header_t hdr);
void       ws_set_len(ws_header_t hdr, uint64_t len);


// to string
void ws_dump_header(ws_header_t hdr, FILE *file);
const char *ws_opcode_tostring(unsigned opcode);
const char *ws_error_tostring(ws_error_t error); 


// masking
void ws_mask_message(ws_mask_t *mask, char *buf, size_t len);
ws_mask_t ws_make_mask(int seed);

// handshake key
#define WS_B64_CONNECTION_KEY_LEN 25 // base64(16 bytes nonce) + null
#define WS_B64_ACCEPT_KEY_LEN     29 // base64(20 bytes sha1 ) + null
ws_error_t ws_make_connection_key(unsigned int seed, char *result, size_t len); 
ws_error_t ws_make_accept_key(char *client_key, char *result, size_t len); 



#endif // __WEBSOCKET_HEADER__
