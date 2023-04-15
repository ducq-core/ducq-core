src=../../src
cmds=../../src/commands
tests=..

mkdir -p build
cd build


rm ./commands/*

gcc -g -c -fPIC -Wall -c ${src}/ducq.c ${src}/ducq_srv.c 

gcc -g -c -fPIC -Wall -c ${cmds}/publish.c
gcc -g -shared -Wl,-Bsymbolic ducq.o ducq_srv.o publish.o -o ./commands/publish.so
gcc -g -c -fPIC -Wall -c ${cmds}/subscribe.c
gcc -g -shared -Wl,-Bsymbolic ducq.o ducq_srv.o subscribe.o -o ./commands/subscribe.so


gcc \
	${src}/ducq.c \
	${src}/ducq_srv.c \
	${tests}/mock_ducq_client.c \
	${tests}/tests_cmd_publish.c \
	${tests}/tests_cmd_subscribe.c \
	${tests}/unit_tests_cmd.c \
	${tests}/tests_cmds.c \
	-o tests_cmds.out \
	-ldl \
	-lcmocka




