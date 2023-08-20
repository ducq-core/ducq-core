#include <string.h>
#include <stdint.h>

#include "base64.h"



static const char B64[] = {
	'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H',
	'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P',
	'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X',
	'Y', 'Z', 'a', 'b', 'c', 'd', 'e', 'f',
	'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n',
	'o', 'p', 'q', 'r', 's', 't', 'u', 'v',
	'w', 'x', 'y', 'z', '0', '1', '2', '3',
	'4', '5', '6', '7', '8', '9', '+', '/'
};

int base64_encode(char *src, int ssize, char *dst, int dsize) {
	if(!src || !dst)
		return -1;

	size_t  len = ssize;
	int     pad = 0;
	int     i   = 0;
	uint8_t x   = 0;

	size_t needed_space = len * 4 / 3;
	pad = needed_space % 4;
	if(pad)	needed_space += 4 - pad;
	needed_space++; // null terminator
	if(dsize < needed_space)
		return -2;
	
	
	pad = len % 3;
	if(pad != 0) {
		len -= pad;
		pad  = 3 - pad;
	}

	for(i = 0; i < len; i += 3) {
		x  = (src[i  ] >> 2) & 0x3F; // 0b111111
		*dst++ = B64[x];

		x  = (src[i  ] << 4) & 0x30; // 0b110000
		x |= (src[i+1] >> 4) & 0x0F; // 0b001111
		*dst++ = B64[x];

		x  = (src[i+1] << 2) & 0x3C; // 0b111100
		x |= (src[i+2] >> 6) & 0x03; // 0b000011
		*dst++ = B64[x];

		x  = (src[i+2] >> 0) & 0x3F; // 0b111111
		*dst++ = B64[x];
	}
	
	if(pad) {
		x  = (src[i  ] >> 2) & 0x3F; // 0b111111
		*dst++ = B64[x];
		
		x  = (src[i  ] << 4) & 0x30; // 0b110000
	}
	if(pad == 1) {
		x |= (src[i+1] >> 4) & 0x0F; // 0b001111
		*dst++ = B64[x];

		x  = (src[i+1] << 2) & 0x3C; // 0b111100
	}
	if(pad)
		*dst++ = B64[x];
	for(i = 0; i < pad; i++){
		*dst++ = '=';}


	*dst = '\0';
	return 0;

}
