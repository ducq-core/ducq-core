set -e


src=../../src
tests=..

mkdir -p build
cd build



gcc \
	${src}/inet_socket.c \
	${tests}/mock_os.c \
	${tests}/tests_inet.c \
	${tests}/main_inet.c \
	-o tests_inet.out \
	-D __unit_tests__ \
	-D read=mock_read \
	-D write=mock_write \
	-D close=mock_close \
	-D getsockname=mock_getsockname \
	-D getpeername=mock_getpeername \
	-lcmocka



valgrind ./tests_inet.out

