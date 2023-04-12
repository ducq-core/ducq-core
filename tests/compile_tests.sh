src=../../src
tests=..

mkdir -p build
cd build

gcc -g -c -fPIC -Wall -c ${src}/ducq.c
mkdir -p ./commands
rm ./commands/*.so
rm ./commands/_not_a_shared_object.txt -f
gcc -g -c -fPIC -Wall -c ${tests}/mock_command_a.c
gcc -g -shared -Wl,-Bsymbolic mock_command_a.o ducq.o -o ./commands/mock_command_a.so
gcc -g -c -fPIC -Wall -c ${tests}/mock_command_b.c
gcc -g -shared -Wl,-Bsymbolic mock_command_b.o ducq.o -o ./commands/mock_command_b.so
gcc -g -c -fPIC -Wall -c ${tests}/mock_command_c.c
gcc -g -shared -Wl,-Bsymbolic mock_command_c.o ducq.o -o ./commands/mock_command_c.so
gcc -g -c -fPIC -Wall -c ${tests}/mock_command_err.c
gcc -g -shared -Wl,-Bsymbolic mock_command_err.o ducq.o -o ./commands/mock_command_err.so

touch ./commands/_not_a_shared_object.txt

	# ${tests}/mock_command.c \

gcc \
	${src}/ducq.c \
	${src}/ducq_tcp.c \
	${src}/ducq_srv.c \
	${tests}/mock_ducq_client.c \
	${tests}/mock_inet.c \
	${tests}/tests_srv_cmd.c \
	${tests}/tests_srv_sub.c \
	${tests}/tests_srv.c \
	${tests}/tests_state.c \
	${tests}/tests_tcp.c \
	${tests}/tests.c \
	-o tests.out \
	-ldl \
	-lcmocka


#################

rm ./commands/*

cmds=../../src/commands
gcc -g -c -fPIC -Wall -c ${src}/ducq.c ${src}/publish.c 

gcc -g -c -fPIC -Wall -c ${cmds}/publish.c
gcc -g -shared -Wl,-Bsymbolic publish.o ducq.o publish.o -o ./commands/publish.so
gcc -g -c -fPIC -Wall -c ${cmds}/subscribe.c
gcc -g -shared -Wl,-Bsymbolic subscribe.o ducq.o subscribe.o -o ./commands/subscribe.so


gcc \
	${src}/ducq.c \
	${src}/ducq_srv.c \
	${tests}/mock_ducq_client.c \
	${tests}/tests_cmds.c \
	-o tests_cmds.out \
	-ldl \
	-lcmocka




