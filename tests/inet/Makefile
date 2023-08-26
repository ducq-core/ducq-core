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
	$(SRCDIR)/inet_ws.c \
	$(SRCDIR)/base64.c \
	$(SRCDIR)/sha1.c \
	$(TESTDIR)/mock_os.c \
	main_inet.c \
	tests_base64.c \
	tests_http.c \
	tests_inet.c \
	tests_ws_handshake.c \
	tests_ws_header.c

inet_internals_test_objs = \
	$(SRCDIR)/inet_ws.c \
	main_ws_internals.c \
	mock_base64.c \
	mock_sha1.c \
	tests_ws_handshake_internals.c

.PHONY: test
test: PREREQ $(inet_test_objs)
	$(CC) -o $(BUILDDIR)/$@ $(inet_test_objs) $(HEADERS) $(RE_DEFINES) $(LINK_TEST_LIB)
	valgrind $(BUILDDIR)/$@
	$(CC) -o $(BUILDDIR)/$@ $(inet_internals_test_objs) $(HEADERS) $(RE_DEFINES) $(LINK_TEST_LIB)
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

