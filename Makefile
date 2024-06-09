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
	$(CC) -g -fPIC -Wall ./src/commands/publish.c   -shared -o $(prefix)/lib/$(cmddir)/publish.so
	$(CC) -g -fPIC -Wall ./src/commands/subscribe.c -shared -o $(prefix)/lib/$(cmddir)/subscribe.so
	$(CC) -g -fPIC -Wall ./src/commands/list_subscriptions.c -shared -o $(prefix)/lib/$(cmddir)/list_subscriptions.so
	$(CC) -g -fPIC -Wall ./src/commands/PING.c -shared -o $(prefix)/lib/$(cmddir)/PING.so
	$(CC) -g -fPIC -Wall ./src/commands/PONG.c -shared -o $(prefix)/lib/$(cmddir)/PONG.so
	$(CC) -g -fPIC -Wall ./src/commands/time.c -shared -o $(prefix)/lib/$(cmddir)/time.so
	$(CC) -g -fPIC -Wall ./src/commands/epoch.c -shared -o $(prefix)/lib/$(cmddir)/epoch.so
	$(CC) -g -fPIC -Wall ./src/commands/list_connections.c -shared -o $(prefix)/lib/$(cmddir)/list_connections.so
	$(CC) -g -fPIC -Wall ./src/commands/unsubscribe.c -shared -o $(prefix)/lib/$(cmddir)/unsubscribe.so
	sudo ldconfig
