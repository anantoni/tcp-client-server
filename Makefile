CC = g++
CFLAGS  = -g -Wall -std=c++11
SOURCES = client_options.cpp client.cpp server_options.cpp server.cpp connection.cpp 

default: remoteClient dataServer

remoteClient:  client.o client_options.o connection.o 
	$(CC) $(CFLAGS) -o remoteClient client.o client_options.o connection.o

dataServer: server.o server_options.o connection.o
	$(CC) $(CFLAGS) -o dataServer server.o server_options.o connection.o

client.o:  client.cpp client_options.h connection.h
	$(CC) $(CFLAGS) -c client.cpp

server.o: server.cpp server_options.h connection.h
	$(CC) $(CFLAGS) -c server.cpp

client_options.o:  client_options.cpp client_options.h 
	$(CC) $(CFLAGS) -c client_options.cpp

server_options.o: server_options.cpp server_options.h
	$(CC) $(CFLAGS) -c server_options.cpp

connection.o:  connection.cpp connection.h
	$(CC) $(CFLAGS) -c connection.cpp

clean: 
	$(RM) count *.o *~

.PHONY: check-syntax
check-syntax:
	$(CC) -Wall -Wextra -std=c++11 -fsyntax-only $(SOURCES)

