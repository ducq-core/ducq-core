#include <stdlib.h>

#include <string.h> // strlen for sha1


#include "inet_ws.h"





//
//			B Y T E   O R D E R I N G
//			

uint16_t ws_reorder_16(uint16_t x) {
	if(WS_IS_NETWORK_BYTE_ORDER) return x;

	uint16_t res = x;
	
	char *ptr = (char*) &res;
	char tmp = ptr[0];
	ptr[0]   = ptr[1];
	ptr[1]   = tmp;

	return res;
}
uint64_t ws_reorder_64(uint64_t x) {
	if(WS_IS_NETWORK_BYTE_ORDER) return x;
	
	uint64_t res = x;
	
	char *ptr = (char*) &res;
	for(int i = 0; i < 4; i++) {
		char tmp   = ptr[  i];
		ptr[  i]   = ptr[7-i];
		ptr[7-i]   = tmp;
	}

	return res;
}




//
//			G E T T E R S   A N D   S E T T E R S
//

ws_mask_t *ws_get_msk(ws_header_t hdr) {
	byte_t len  =  hdr[1] & WS_LEN;
	byte_t *ptr = &hdr[2];
	return (ws_mask_t *)
	(
		len == 127 ? ptr + sizeof(uint64_t) : 
		len == 126 ? ptr + sizeof(uint16_t) : ptr
	);
}

void ws_set_msk(ws_header_t hdr, void *mask) {
	hdr[1] |= WS_MASK;
	ws_mask_t *msk = ws_get_msk(hdr);

	msk->u32 = * (uint32_t*) mask;
}
uint64_t ws_get_len(ws_header_t hdr) {
	uint64_t len =  hdr[1] & WS_LEN;
	byte_t  *ptr = &hdr[2];

	return	len == 126 ? ws_reorder_16( *(uint16_t*)ptr ) :
		len == 127 ? ws_reorder_64( *(uint64_t*)ptr ) : len;
}
void ws_set_len(ws_header_t hdr, uint64_t len) {
	bool is_masked = hdr[1] & WS_MASK;
	ws_mask_t mask = *ws_get_msk(hdr); // copy

	byte_t  *ptr = &hdr[2];

	if(len > UINT16_MAX) { 
		hdr[1] = 127;
		*(uint64_t*)ptr = ws_reorder_64(len);
	} 
	else if(len > 125) { 
		hdr[1] = 126;
		*(uint16_t*)ptr = ws_reorder_16(len); 
	} 
	else {
		hdr[1] = len;
	}

	if(is_masked)
		ws_set_msk(hdr, &mask); 
}


size_t ws_get_hdr_len(ws_header_t hdr) {
	size_t len = 2;

	switch (hdr[1] & WS_LEN) {
		case 126: len += sizeof(uint16_t); break;
		case 127: len += sizeof(uint64_t); break;
	}
	
	if (hdr[1] & WS_MASK)
		len += sizeof(ws_mask_t);

	return len;
}



//
//			T O   S T R I N G
//

#define WS_FAIL(error) ( ws->state = WS_FAILED, return error )

const char *ws_opcode_tostring(unsigned opcode) {
        if(opcode == 0 || opcode > 0xF) return "invalid";
        switch(opcode) {
                case WS_CONTINUATION: return "continuation";
                case WS_TEXT        : return "text";
                case WS_BINARY      : return "binary";
                case WS_CLOSE       : return "close";
                case WS_PING        : return "ping";
                case WS_PONG        : return "pong";
                default:              return "reserved";
        };
}

const char *ws_error_tostring(ws_error_t error) {
        switch(error) {
                case WS_OK             : return "success";

                case WS_BASE64_NULL    : return "base64 null pointer parameter";
                case WS_BASE64_OUT_LEN : return "base64 output buffer to short";

		case WS_SHA1_NULL      : return "sha1 null pointer parameter";
                case WS_SHA1_IN_LEN    : return "sha1 input data too long";
                case WS_SHA1_STATE     : return "sha1 called Input after Result";
                
		case WS_NULL_PARAM     : return "output buffer is null";
		case WS_OUT_LEN        : return "output buffer to short";

                default                : return "unknown";
        };
}


static
void _dump_memory(void *mem, size_t len, FILE *file) {
	static unsigned int packet_len = 32;
	unsigned char *ptr = mem;

	fprintf(file, "     ");
	for(int i = 0; i < packet_len; i++)
		fprintf(file, "%02d ", i);

	for(size_t i = 0; i < len; i++) {
		if(i%packet_len == 0)
			fprintf(file, "\n%04x ", (unsigned int)(i/packet_len));
		fprintf(file, "%02x ", ptr[i]);
	}

	fprintf(file, "\n");
}

void ws_dump_header(ws_header_t hdr, FILE *file) {
	fprintf(file, "in %s\n", __func__);
	_dump_memory(hdr, WS_HEADER_SIZE, file);

	fprintf(file, "fin : %s\n", (hdr[0] & WS_FIN ) ? "true" : "false");
	fprintf(file, "rsv1: %s\n", (hdr[0] & WS_RSV1) ? "true" : "false");
	fprintf(file, "rsv2: %s\n", (hdr[0] & WS_RSV2) ? "true" : "false");
	fprintf(file, "rsv3: %s\n", (hdr[0] & WS_RSV3) ? "true" : "false");

	byte_t opcode = hdr[0] & WS_OPCODE;
	fprintf(file, "code: %s (%d)\n", ws_opcode_tostring(opcode), opcode);
	
	fprintf(file, "mask: ");
	if( !(hdr[1] & WS_MASK) )
		fprintf(file, "false\n");
	else {
		ws_mask_t *mask = ws_get_msk(hdr);
		fprintf(file, "0x%02x 0x%02x 0x%02x 0x%02x\n", mask->bytes[0], mask->bytes[1], mask->bytes[2], mask->bytes[3]);
	}
	
	fprintf(file, "len : %lu\n", ws_get_len(hdr));
}



//
//			M A S K I N G
//

void ws_mask_message(ws_mask_t *mask, char *buf, size_t len) {
	for(size_t i = 0; i < len; i++)
		buf[i] ^=  mask->bytes[i%4];		
}

ws_mask_t ws_make_mask(int seed) {
	ws_mask_t mask = {};

	srand(seed);

	mask.bytes[0] = rand();
	mask.bytes[1] = rand();
	mask.bytes[2] = rand();
	mask.bytes[3] = rand();

	return mask;
}


//
//			H A N D S H A K E   K E Y
//

#include "sha1.h"
#include "base64.h"

static
ws_error_t _sha1err_to_ws_err(int sha1err) {
	switch(sha1err) {
		case shaNull         : return WS_SHA1_NULL;
		case shaInputTooLong : return WS_SHA1_IN_LEN;
		case shaStateError   : return WS_SHA1_STATE;
		default              : return WS_OK;
	}
}

static
ws_error_t _base64err_to_ws_err(int base64err) {
	switch(base64err) {
		case -1 : return WS_BASE64_NULL;
		case -2 : return WS_BASE64_OUT_LEN;
		default : return WS_OK;
	}
}

ws_error_t ws_make_connection_key(unsigned int seed, char *result, size_t len) {
	if(!result) return WS_NULL_PARAM;

	byte_t nonce[16] = {};
	srand(seed);

	for(int i = 0; i < sizeof(nonce); i++)
		nonce[i] = rand();

	int b64_err = base64_encode(nonce, sizeof(nonce), result, len);
        if (b64_err) return _base64err_to_ws_err(b64_err);	

	return WS_OK;
} 


ws_error_t ws_make_accept_key(char *client_key, char *result, size_t len) {
	if(!client_key || !result) return WS_NULL_PARAM;
	
	char buffer[WS_B64_CONNECTION_KEY_LEN + sizeof(WS_GUID)] = "";
	static size_t buffer_size = sizeof(buffer)-2; // should be exaclty connect-key + guid
	size_t n = snprintf(buffer, sizeof(buffer), "%s%s", client_key, WS_GUID);
        if(n != buffer_size) return WS_IN_LEN;

//	sha1          
        SHA1Context sha1 = {};
        int sha_err = 0;
        char sha1_result[SHA1HashSize] = "";
        
        if ( sha_err = SHA1Reset(&sha1)                      ) return _sha1err_to_ws_err(sha_err);
        if ( sha_err = SHA1Input(&sha1, buffer, buffer_size) ) return _sha1err_to_ws_err(sha_err);
        if ( sha_err = SHA1Result(&sha1, sha1_result)        ) return _sha1err_to_ws_err(sha_err);

//	base64
        int b64_err = base64_encode(sha1_result, SHA1HashSize, result, len);
        if (b64_err) return _base64err_to_ws_err(b64_err);

	return WS_OK;
}

