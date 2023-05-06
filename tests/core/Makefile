
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

$(BUILDDIR)/commands/%.so: $(BUILDDIR)/commands/%.o $(BUILDDIR)/ducq.o $(BUILDDIR)/ducq_srv.o
	$(CC) -g -shared -Wl,-Bsymbolic $^ -o $@

mock_commands = \
	$(BUILDDIR)/commands/mock_command_a.so \
	$(BUILDDIR)/commands/mock_command_b.so \
	$(BUILDDIR)/commands/mock_command_c.so \
	$(BUILDDIR)/commands/mock_command_err.so 
core_test_objs= \
	${SRCDIR}/ducq.c \
	${SRCDIR}/ducq_tcp.c \
	${SRCDIR}/ducq_srv.c \
	${TESTDIR}/mock_ducq_client.c \
	mock_inet.c \
	tests_srv_cmd.c \
	tests_srv_sub.c \
	tests_srv_parse.c \
	tests_state.c \
	tests_tcp.c \
	main.c
.PHONY: tests_core
tests_core: PREREQ $(mock_commands) $(core_test_objs)
	$(CC) -o $(BUILDDIR)/$@ $(core_test_objs) $(HEADERS) -ldl $(LINK_TEST_LIB)
	cd $(BUILDDIR) &&  valgrind ./$@


.PHONY: PREREQ
PREREQ:
	mkdir -p build/commands

.PHONY: clean
clean:
	rm -rf build