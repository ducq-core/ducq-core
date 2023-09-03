
CC = gcc

SRCDIR   = ../../src
SRC_CMDDIR = $(SRCDIR)/commands
TESTDIR  = ..
BUILDDIR = ./build

HEADERS = -I $(SRCDIR) -I $(TESTDIR)

LINK_TEST_LIB = -lcmocka


$(BUILDDIR)/%.o: $(SRCDIR)/%.c
	$(CC) -c $< -o $@

lua_test_objs= \
	${SRCDIR}/ducq.c \
	${SRCDIR}/ducq_reactor.c \
	${SRCDIR}/ducq_dispatcher.c \
	${SRCDIR}/ducq_lua.c \
	${TESTDIR}/mock_ducq_client.c \
	tests_lua.c \
	tests_lua_reactor.c \
	tests_lua_client_iterator.c \
	main.c


.PHONY: tests_lua
tests_lua: PREREQ $(lua_test_objs)
	$(CC) -g -fPIC -shared  $(SRCDIR)/ducq_lua.c -o $(BUILDDIR)/LuaDucq-Test.so
	$(CC) -o $(BUILDDIR)/$@ $(lua_test_objs) $(HEADERS) -ldl -llua -lm $(LINK_TEST_LIB) -rdynamic -DLUA_DUCQ_PACKAGE_NAME=\"LuaDucq-Test\"
	cd $(BUILDDIR) &&  valgrind ./$@


.PHONY: PREREQ
PREREQ:
	mkdir -p build

.PHONY: clean
clean:
	rm -rf build
