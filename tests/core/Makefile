
CC = gcc

SRCDIR   = ../../src
SRC_CMDDIR = $(SRCDIR)/commands
TESTDIR  = ..
BUILDDIR = ./build

HEADERS = -I $(SRCDIR) -I $(TESTDIR)

LINK_TEST_LIB = -lcmocka


$(BUILDDIR)/%.o: $(SRCDIR)/%.c
	$(CC) -c $< -o $@

$(BUILDDIR)/commands/%.o: %.c
	$(CC) -g -c -fPIC -Wall -c $< -o $@ $(HEADERS)

$(BUILDDIR)/commands/%.so: $(BUILDDIR)/commands/%.o $(BUILDDIR)/ducq.o $(BUILDDIR)/ducq_reactor.o $(BUILDDIR)/ducq_dispatcher.o
	$(CC) -g -shared -Wl,-Bsymbolic $^ -o $@

mock_commands = \
	$(BUILDDIR)/commands/mock_command_a.so \
	$(BUILDDIR)/commands/mock_command_b.so \
	$(BUILDDIR)/commands/mock_command_c.so \
	$(BUILDDIR)/commands/mock_command_err.so 
core_test_objs= \
	${SRCDIR}/ducq.c \
	${SRCDIR}/ducq_tcp.c \
	${SRCDIR}/sha1.c \
	${SRCDIR}/base64.c \
	${SRCDIR}/inet_ws.c \
	${SRCDIR}/inet_http.c \
	${SRCDIR}/ducq_ws.c \
	${SRCDIR}/ducq_reactor.c \
	${SRCDIR}/ducq_dispatcher.c \
	${SRCDIR}/ducq_lua.c \
	${TESTDIR}/mock_ducq_client.c \
	mock_inet.c \
	tests_dispatcher.c \
	tests_dispatcher_lua.c \
	tests_connections.c \
	tests_iterator.c \
	tests_ack.c \
	tests_log.c \
	tests_parse.c \
	tests_state.c \
	tests_tcp.c \
	tests_ws.c \
	tests_cli_publish.c \
	tests_cli_subscribe.c \
	main.c
core_test_http_objs= \
	${SRCDIR}/ducq.c \
	${SRCDIR}/sha1.c \
	${SRCDIR}/base64.c \
	${SRCDIR}/inet_ws.c \
	${SRCDIR}/ducq_ws.c \
	${SRCDIR}/ducq_http.c \
	${SRCDIR}/inet_socket.c \
	${SRCDIR}/inet_http.c \
	${TESTDIR}/mock_os.c \
	tests_http.c \
	main_http.c

.PHONY: tests_core
tests_core: PREREQ $(mock_commands) $(core_test_objs)
	$(CC) -g -fPIC -shared  $(SRCDIR)/ducq_lua.c -o $(BUILDDIR)/LuaDucq-Test.so
	$(CC) -o $(BUILDDIR)/$@ $(core_test_objs) $(HEADERS) -ldl -llua -lm $(LINK_TEST_LIB) -rdynamic -DLUA_DUCQ_PACKAGE_NAME=\"LuaDucq-Test\"
	cp *.lua $(BUILDDIR)
	cd $(BUILDDIR) &&  valgrind ./$@
	$(CC) -o $(BUILDDIR)/$@ $(core_test_http_objs) $(HEADERS) $(RE_DEFINES) $(LINK_TEST_LIB)
	cd $(BUILDDIR) &&  valgrind ./$@

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
