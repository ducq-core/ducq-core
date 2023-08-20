CC = gcc

SRCDIR   = ../../src
SRC_CMDDIR = $(SRCDIR)/commands
TESTDIR  = ..
BUILDDIR = ./build

HEADERS = -I $(SRCDIR) -I $(TESTDIR)
LINK_TEST_LIB = -lcmocka

inet_test_objs = \
	$(SRCDIR)/inet_socket.c \
	$(SRCDIR)/inet_http.c \
	$(SRCDIR)/base64.c \
	*.c

.PHONY: test
test: PREREQ $(inet_test_objs)
	$(CC) -o $(BUILDDIR)/$@ $(inet_test_objs) $(HEADERS) $(RE_DEFINES) $(LINK_TEST_LIB)
	valgrind $(BUILDDIR)/$@



RE_DEFINES = \
	-D read=mock_read \
	-D write=mock_write \
	-D close=mock_close \
	-D getsockname=mock_getsockname \
	-D getpeername=mock_getpeername


.PHONY: PREREQ
PREREQ:
	mkdir -p build/commands

.PHONY: clean
clean:
	rm -rf build

