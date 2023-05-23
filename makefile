CC = gcc
CFLAGS = -g -Wall
LIBS = -lpthread
SHAREDLIB = -shared
RM = rm -f

all: react_server

react_server: react_server.o libreactor.so 
	$(CC) $(CFLAGS) -o react_server react_server.o -L. -lreactor

react_server.o: server.c reactor.h
	$(CC) $(CFLAGS) -c server.c -o react_server.o

libreactor.so: reactor.o
	$(CC) $(CFLAGS) $(SHAREDLIB) -o libreactor.so reactor.o

reactor.o: reactor.c reactor.h 
	$(CC) $(CFLAGS) -fPIC -c reactor.c

clean:
	$(RM) react_server react_server.o reactor.o libreactor.so

