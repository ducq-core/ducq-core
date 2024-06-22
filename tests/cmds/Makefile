
CC = gcc

SRCDIR   = ../../src
SRC_CMDDIR = $(SRCDIR)/commands
TESTDIR  = ..
BUILDDIR = ./build

HEADERS = -I $(SRCDIR) -I $(TESTDIR)

LINK_TEST_LIB = -lcmocka



cmd_test_objs= \
	$(BUILDDIR)/commands/pub.so \
	$(BUILDDIR)/commands/sub.so \
	$(BUILDDIR)/commands/lsconn.so \
	$(BUILDDIR)/commands/unsub.so \
	${SRCDIR}/ducq.c \
	${SRCDIR}/ducq_log.c \
	${SRCDIR}/ducq_reactor.c \
	${SRCDIR}/ducq_dispatcher.c \
	${SRCDIR}/ducq_lua.c \
	${TESTDIR}/*.c \
	*.c

.PHONY: test_cmds
tests_cmds: PREREQ  $(cmd_test_objs)
	cp *.lua $(BUILDDIR)/commands/
	$(CC) -o $(BUILDDIR)/$@ $(cmd_test_objs) $(HEADERS) -ldl -llua -lm -rdynamic $(LINK_TEST_LIB) -Wall
	cd $(BUILDDIR)/commands/ && cp \
		sub.so \
		pub.so \
		lsconn.so \
		lua_command.lua \
		../list_commands
	cd $(BUILDDIR) && valgrind ./$@


$(BUILDDIR)/%.o: $(SRCDIR)/%.c
	$(CC) -c $< -o $@

$(BUILDDIR)/commands/%.o: $(SRC_CMDDIR)/%.c
	$(CC) -g -c -fPIC -Wall -c $< -o $@

$(BUILDDIR)/commands/%.so: $(BUILDDIR)/commands/%.o
	$(CC) -g -shared -Wl,-Bsymbolic $^ -o $@


.PHONY: PREREQ
PREREQ:
	mkdir -p $(BUILDDIR)/commands
	mkdir -p $(BUILDDIR)/list_commands

.PHONY: clean
clean:
	rm -rf build

