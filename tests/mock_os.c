#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <stdint.h>
#include <cmocka.h>

#include <string.h>

#include "mock_os.h"


struct sockaddr_storage MOCK_ADDR;
socklen_t MOCK_ADDR_LEN;


ssize_t mock_read(int fd, void *buf, size_t count) {
	check_expected(fd);
	check_expected(buf);
	check_expected(count);
	return mock();
}
ssize_t mock_write(int fd, const void *buf, size_t count) {
	check_expected(fd);
	check_expected(buf);
	check_expected(count);
	return mock();
}

int mock_close(int fd) {
	check_expected(fd);
	return mock();
}

int mock_getsockname(int fd, struct sockaddr *addr, socklen_t *len) {
	check_expected(fd);

	memcpy(addr, &MOCK_ADDR, MOCK_ADDR_LEN);
	*len = MOCK_ADDR_LEN;

	return mock();
}

int mock_getpeername(int fd, struct sockaddr *addr, socklen_t *len) {
	check_expected(fd);

	memcpy(addr, &MOCK_ADDR, MOCK_ADDR_LEN);
	*len = MOCK_ADDR_LEN;

	return mock();
}