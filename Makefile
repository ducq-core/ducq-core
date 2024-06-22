prefix = /usr/local
cmddir = ducq_commands
CC = gcc

SOURCES = $(wildcard ./src/.*c)
HEADERS = $(wildcard ./src/.*h)


install:
	mkdir -p build
	mkdir -p $(prefix)/lib
	$(CC) -g -fPIC -Wall ./src/ducq_lua.c -shared -o $(prefix)/lib/lua/5.4/LuaDucq.so
	$(CC) -g -fPIC -Wall ./src/*.c -shared -o $(prefix)/lib/libducq.so -ldl
	cp ./src/*.h $(prefix)/include
	sudo ldconfig


install_commands:
	mkdir -p $(prefix)/lib/$(cmddir)
	$(CC) -g -fPIC -Wall ./src/commands/pub.c   -shared -o $(prefix)/lib/$(cmddir)/pub.so
	$(CC) -g -fPIC -Wall ./src/commands/sub.c -shared -o $(prefix)/lib/$(cmddir)/sub.so
	$(CC) -g -fPIC -Wall ./src/commands/PING.c -shared -o $(prefix)/lib/$(cmddir)/PING.so
	$(CC) -g -fPIC -Wall ./src/commands/PONG.c -shared -o $(prefix)/lib/$(cmddir)/PONG.so
	$(CC) -g -fPIC -Wall ./src/commands/time.c -shared -o $(prefix)/lib/$(cmddir)/time.so
	$(CC) -g -fPIC -Wall ./src/commands/epoch.c -shared -o $(prefix)/lib/$(cmddir)/epoch.so
	$(CC) -g -fPIC -Wall ./src/commands/lsconn.c -shared -o $(prefix)/lib/$(cmddir)/lsconn.so
	$(CC) -g -fPIC -Wall ./src/commands/unsub.c -shared -o $(prefix)/lib/$(cmddir)/unsub.so
	sudo ldconfig
