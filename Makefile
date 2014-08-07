CC     := g++
PORT   := 8502
CFLAGS  = -g -Wall -Wextra -std=c++11
SOURCES = client_options.cpp client.cpp server_options.cpp server.cpp connection.cpp 


default: remoteClient dataServer

remoteClient:  client.o client_options.o connection.o 
	$(CC) $(CFLAGS) -o remoteClient client.o client_options.o connection.o

dataServer: server.o server_options.o connection.o request_handler.o task.o task_queue.o worker.o
	$(CC) $(CFLAGS) -o dataServer server.o server_options.o connection.o request_handler.o task.o task_queue.o worker.o -lpthread

client.o:  client.cpp client_options.hpp connection.hpp
	$(CC) $(CFLAGS) -c client.cpp

server.o: server.cpp server_options.hpp connection.hpp
	$(CC) $(CFLAGS) -c server.cpp

request_handler.o: request_handler.cpp request_handler.hpp
	$(CC) $(CFLAGS) -c request_handler.cpp

task.o: task.cpp task.hpp
	$(CC) $(CFLAGS) -c task.cpp

worker.o: worker.cpp worker.hpp
	$(CC) $(CFLAGS) -c worker.cpp

task_queue.o: task_queue.cpp task_queue.hpp
	$(CC) $(CFLAGS) -c task_queue.cpp

client_options.o:  client_options.cpp client_options.hpp
	$(CC) $(CFLAGS) -c client_options.cpp

server_options.o: server_options.cpp server_options.hpp
	$(CC) $(CFLAGS) -c server_options.cpp

connection.o:  connection.cpp connection.hpp
	$(CC) $(CFLAGS) -c connection.cpp

clean: 
	$(RM) count *.o *~

.PHONY: check-syntax
check-syntax:
	$(CC) $(CFLAGS) -fsyntax-only $(SOURCES)

.PHONY: deploy
deploy: dataServer
	./dataServer -p $(PORT) -s 4 -q 6

.PHONY: connect
connect: remoteClient
	./remoteClient -i 127.0.0.1 -p $(PORT) -d test
