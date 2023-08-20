#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <stdint.h>
#include <cmocka.h>

#include "sha1.h"



int SHA1Reset(SHA1Context *) {
	return mock();
}
int SHA1Input(SHA1Context *, const uint8_t *, unsigned int) {
	return mock();
}
int SHA1Result(SHA1Context *, uint8_t Message_Digest[SHA1HashSize]) {
	return mock();
}


