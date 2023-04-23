set -e

src=../../src
tests=..

#rm -r build/
mkdir -p build
cd build

gcc -g -c -fPIC -Wall -c ${src}/ducq.c
mkdir -p ./commands
rm ./commands/*.so -f
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
	${tests}/tests_srv_parse.c \
	${tests}/tests_state.c \
	${tests}/tests_tcp.c \
	${tests}/main.c \
	-o tests.out \
	-ldl \
	-lcmocka




valgrind ./tests.out
