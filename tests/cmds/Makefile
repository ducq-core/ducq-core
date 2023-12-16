
CC = gcc

SRCDIR   = ../../src
SRC_CMDDIR = $(SRCDIR)/commands
TESTDIR  = ..
BUILDDIR = ./build

HEADERS = -I $(SRCDIR) -I $(TESTDIR)

LINK_TEST_LIB = -lcmocka



cmd_test_objs= \
	$(BUILDDIR)/commands/publish.so \
	$(BUILDDIR)/commands/subscribe.so \
	$(BUILDDIR)/commands/list_subscriptions.so \
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
	$(CC) -o $(BUILDDIR)/$@ $(cmd_test_objs) $(HEADERS) -ldl -llua -lm -rdynamic $(LINK_TEST_LIB)
	cd $(BUILDDIR) && valgrind ./$@


$(BUILDDIR)/%.o: $(SRCDIR)/%.c
	$(CC) -c $< -o $@

$(BUILDDIR)/commands/%.o: $(SRC_CMDDIR)/%.c
	$(CC) -g -c -fPIC -Wall -c $< -o $@

$(BUILDDIR)/commands/%.so: $(BUILDDIR)/commands/%.o
	$(CC) -g -shared -Wl,-Bsymbolic $^ -o $@


.PHONY: PREREQ
PREREQ:
	mkdir -p build/commands

.PHONY: clean
clean:
	rm -rf build

