
CC = gcc

SRCDIR   = ../src
SRC_CMDDIR = $(SRCDIR)/commands
TESTDIR  = .
BUILDDIR = ./build

HEADERS = -I $(SRCDIR) -I $(TESTDIR)

LINK_TEST_LIB = -lcmocka


tests_all: tests_core tests_inet tests_cmds

.PHONY: PREREQ
PREREQ:
	rm -rf build
	mkdir -p build/commands



# I N E T

INET_DIR=./inet
inet_test_objs = $(SRCDIR)/inet_socket.c $(INET_DIR)/main_inet.c $(INET_DIR)/mock_os.c $(INET_DIR)/tests_inet.c
RE_DEFINES = \
	-D read=mock_read \
	-D write=mock_write \
	-D close=mock_close \
	-D getsockname=mock_getsockname \
	-D getpeername=mock_getpeername

.PHONY: tests_inet
tests_inet: PREREQ $(inet_test_objs)
	$(CC) -o $(BUILDDIR)/$@ $(inet_test_objs) $(HEADERS) $(RE_DEFINES) $(LINK_TEST_LIB)
	valgrind $(BUILDDIR)/$@



# C O M M A N D S 

CMDS_DIR=./cmds
$(BUILDDIR)/%.o: $(SRCDIR)/%.c
	$(CC) -c $< -o $@

$(BUILDDIR)/commands/%.o: $(SRC_CMDDIR)/%.c
	$(CC) -g -c -fPIC -Wall -c $< -o $@

$(BUILDDIR)/commands/%.so: $(BUILDDIR)/commands/%.o $(BUILDDIR)/ducq.o $(BUILDDIR)/ducq_srv.o
	$(CC) -g -shared -Wl,-Bsymbolic $^ -o $@

cmd_test_objs= ${SRCDIR}/ducq.c \
	${SRCDIR}/ducq_srv.c \
	${TESTDIR}/mock_ducq_client.c \
	${TESTDIR}/unit_tests_cmd.c \
	${CMDS_DIR}/tests_cmd_publish.c \
	${CMDS_DIR}/tests_cmd_subscribe.c \
	${CMDS_DIR}/main_cmds.c
.PHONY: test_cmds
tests_cmds: PREREQ $(BUILDDIR)/commands/publish.so $(BUILDDIR)/commands/subscribe.so $(cmd_test_objs)
	$(CC) -o $(BUILDDIR)/$@ $(cmd_test_objs) $(HEADERS) -ldl $(LINK_TEST_LIB)
	cd $(BUILDDIR) && valgrind ./$@



# C O R E
CORE_DIR=./core

$(BUILDDIR)/commands/%.o: $(CORE_DIR)/%.c
	$(CC) -g -c -fPIC -Wall -c $< -o $@ $(HEADERS)

$(BUILDDIR)/commands/%.so: $(BUILDDIR)/commands/%.o $(BUILDDIR)/ducq.o $(BUILDDIR)/ducq_srv.o
	$(CC) -g -shared -Wl,-Bsymbolic $^ -o $@

mock_commands = $(BUILDDIR)/commands/mock_command_a.so \
	$(BUILDDIR)/commands/mock_command_b.so \
	$(BUILDDIR)/commands/mock_command_c.so \
	$(BUILDDIR)/commands/mock_command_err.so 
core_test_objs= 	${SRCDIR}/ducq.c \
	${SRCDIR}/ducq_tcp.c \
	${SRCDIR}/ducq_srv.c \
	${TESTDIR}/mock_ducq_client.c \
	${CORE_DIR}/mock_inet.c \
	${CORE_DIR}/tests_srv_cmd.c \
	${CORE_DIR}/tests_srv_sub.c \
	${CORE_DIR}/tests_srv_parse.c \
	${CORE_DIR}/tests_state.c \
	${CORE_DIR}/tests_tcp.c \
	${CORE_DIR}/main.c
.PHONY: tests_core
tests_core: PREREQ $(mock_commands) $(core_test_objs)
	$(CC) -o $(BUILDDIR)/$@ $(core_test_objs) $(HEADERS) -ldl $(LINK_TEST_LIB)
	cd $(BUILDDIR) &&  valgrind ./$@
