
CC=gcc
src_dir=../src
build_dir=./build
FLAGS = -lcmocka



inet_obj      = $(src_dir)/inet_socket.o
inet_test_obj = mock_os.o tests_inet.o main_inet.o 
DEFINES = \
	-D __unit_tests__ \
	-D read=mock_read \
	-D write=mock_write \
	-D close=mock_close \
	-D getsockname=mock_getsockname \
	-D getpeername=mock_getpeername

tests_inet: $(inet_obj) $(inet_test_obj)
	mkdir -p $(build_dir)
	cd $(build_dir)
	$(CC) -o $@ $^ $(DEFINES) $(FLAGS)
	valgrind $@