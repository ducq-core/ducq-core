#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <setjmp.h>
#include <cmocka.h>

#include <stdio.h>
#include <string.h>

#include "base64.h"


void test_hello_world(void **state) {
	// arrange
	(void) state; /* unused */
	char text[] = "hello, world";
	char actual[1024] = "";
	char expected[] = "aGVsbG8sIHdvcmxk";

	// act
	base64_encode(text, strlen(text), actual, sizeof(actual));

	// assert
	assert_string_equal(actual, expected);
}

void test_one_padding(void **state) {
	// arrange
	(void) state; /* unused */
	char text[] = "basetest";
	char actual[1024] = "";
	char expected[] = "YmFzZXRlc3Q=";

	// act
	base64_encode(text, strlen(text), actual, sizeof(actual));

	// assert
	assert_string_equal(actual, expected);
}
void test_two_padding(void **state) {
	// arrange
	(void) state; /* unused */
	char text[] = "base64test";
	char actual[1024] = "";
	char expected[] = "YmFzZTY0dGVzdA==";

	// act
	base64_encode(text, strlen(text), actual, sizeof(actual));

	// assert
	assert_string_equal(actual, expected);
}
void test_long_text(void **state) {
	// arrange
	(void) state; /* unused */
	char text[] = "long text to encode in base 64 !";
	char actual[1024] = "";
	char expected[] = "bG9uZyB0ZXh0IHRvIGVuY29kZSBpbiBiYXNlIDY0ICE=";

	// act
	base64_encode(text, strlen(text), actual, sizeof(actual));

	// assert
	assert_string_equal(actual, expected);
}
void test_enough_space(void **state) {
	// arrange
	(void) state; /* unused */
	char text[] = "long text to encode in base 64 !";
	char dst[1024] = "";
	int expected = 0;

	// act
	int actual = base64_encode(text, strlen(text), dst, sizeof(dst));

	// assert
	assert_int_equal(actual, expected);
}
void test_not_enough_space(void **state) {
	// arrange
	(void) state; /* unused */
	char text[] = "long text to encode in base 64 !";
	char dst[10] = "";
	int expected = -2;

	// act
	int actual = base64_encode(text, strlen(text), dst, sizeof(dst));

	// assert
	assert_int_equal(actual, expected);
}
void test_wiki_3(void **state) {
	// arrange
	(void) state; /* unused */
	char text[] = "Man";
	char actual[10] = "";
	char expected[] = "TWFu";

	// act
	base64_encode(text, strlen(text), actual, sizeof(actual));

	// assert
	assert_string_equal(actual, expected);
}
void test_wiki_2(void **state) {
	// arrange
	(void) state; /* unused */
	char text[] = "Ma";
	char actual[10] = "";
	char expected[] = "TWE=";

	// act
	base64_encode(text, strlen(text), actual, sizeof(actual));

	// assert
	assert_string_equal(actual, expected);
}
void test_wiki_1(void **state) {
	// arrange
	(void) state; /* unused */
	char text[] = "M";
	char actual[10] = "";
	char expected[] = "TQ==";

	// act
	base64_encode(text, strlen(text), actual, sizeof(actual));

	// assert
	assert_string_equal(actual, expected);
}
