prefix = /usr/local
cmddir = ducq_commands
CC = gcc

SOURCES = $(wildcard ./src/.*c)
HEADERS = $(wildcard ./src/.*h)


install:
	mkdir -p build
	mkdir -p $(prefix)/lib
	$(CC) -g -fPIC -Wall ./src/*.c -shared -o $(prefix)/lib/libducq.so -ldl
	cp ./src/*.h $(prefix)/include
	sudo ldconfig


install_commands:
	mkdir -p $(prefix)/lib/$(cmddir)
	$(CC) -g -fPIC -Wall ./src/commands/publish.c   -shared -o $(prefix)/lib/$(cmddir)/publish.so
	$(CC) -g -fPIC -Wall ./src/commands/subscribe.c -shared -o $(prefix)/lib/$(cmddir)/subscribe.so
	sudo ldconfig