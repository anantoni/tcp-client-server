CC     		:= g++
PORT   		:= 51001
CFLAGS  	:= -g -Wall -Wextra -std=c++11 -pedantic
SERVER_DIR 	:= server/
CLIENT_DIR 	:= client/
UTILS_DIR 	:= utils/
RM 		  	:= rm
SOURCES 	:= $(CLIENT_DIR)*.cpp $(SERVER_DIR)*.cpp $(UTILS_DIR)*.cpp 


default: $(CLIENT_DIR)remoteClient $(SERVER_DIR)dataServer

$(CLIENT_DIR)remoteClient:  $(CLIENT_DIR)client.o $(CLIENT_DIR)client_options.o $(UTILS_DIR)connection.o $(UTILS_DIR)filesystem.o 
	$(CC) $(CFLAGS) -o $(CLIENT_DIR)remoteClient $(CLIENT_DIR)client.o $(CLIENT_DIR)client_options.o $(UTILS_DIR)connection.o $(UTILS_DIR)filesystem.o

$(SERVER_DIR)dataServer: $(SERVER_DIR)server.o $(SERVER_DIR)server_options.o $(UTILS_DIR)connection.o $(SERVER_DIR)request_handler.o $(UTILS_DIR)task.o $(UTILS_DIR)task_queue.o $(SERVER_DIR)worker.o $(UTILS_DIR)signal_handler.o
	$(CC) $(CFLAGS) -o $(SERVER_DIR)dataServer $(SERVER_DIR)server.o $(SERVER_DIR)server_options.o $(UTILS_DIR)connection.o $(SERVER_DIR)request_handler.o $(UTILS_DIR)task.o $(UTILS_DIR)task_queue.o $(SERVER_DIR)worker.o $(UTILS_DIR)signal_handler.o -lpthread

$(CLIENT_DIR)client.o:  $(CLIENT_DIR)client.cpp $(CLIENT_DIR)client_options.hpp $(UTILS_DIR)connection.hpp
	$(CC) $(CFLAGS) -c $(CLIENT_DIR)client.cpp -o $(CLIENT_DIR)client.o

$(SERVER_DIR)server.o: $(SERVER_DIR)server.cpp $(SERVER_DIR)server_options.hpp $(UTILS_DIR)connection.hpp
	$(CC) $(CFLAGS) -c $(SERVER_DIR)server.cpp -o $(SERVER_DIR)server.o

$(UTILS_DIR)filesystem.o: $(UTILS_DIR)filesystem.cpp $(UTILS_DIR)filesystem.hpp
	$(CC) $(CFLAGS) -c $(UTILS_DIR)filesystem.cpp -o $(UTILS_DIR)filesystem.o

$(SERVER_DIR)request_handler.o: $(SERVER_DIR)request_handler.cpp $(SERVER_DIR)request_handler.hpp
	$(CC) $(CFLAGS) -c $(SERVER_DIR)request_handler.cpp -o $(SERVER_DIR)request_handler.o

$(UTILS_DIR)task.o: $(UTILS_DIR)task.cpp $(UTILS_DIR)task.hpp
	$(CC) $(CFLAGS) -c $(UTILS_DIR)task.cpp -o $(UTILS_DIR)task.o

$(UTILS_DIR)signal_handler.o: $(UTILS_DIR)signal_handler.cpp $(UTILS_DIR)signal_handler.hpp
	$(CC) $(CFLAGSF) -c $(UTILS_DIR)signal_handler.cpp -o $(UTILS_DIR)signal_handler.o

$(SERVER_DIR)worker.o: $(SERVER_DIR)worker.cpp $(SERVER_DIR)worker.hpp
	$(CC) $(CFLAGS) -c $(SERVER_DIR)worker.cpp -o $(SERVER_DIR)worker.o

$(UTILS_DIR)task_queue.o: $(UTILS_DIR)task_queue.cpp $(UTILS_DIR)task_queue.hpp
	$(CC) $(CFLAGS) -c $(UTILS_DIR)task_queue.cpp -o $(UTILS_DIR)task_queue.o

$(CLIENT_DIR)client_options.o:  $(CLIENT_DIR)client_options.cpp $(CLIENT_DIR)client_options.hpp
	$(CC) $(CFLAGS) -c $(CLIENT_DIR)client_options.cpp -o $(CLIENT_DIR)client_options.o 

$(SERVER_DIR)server_options.o: $(SERVER_DIR)server_options.cpp $(SERVER_DIR)server_options.hpp
	$(CC) $(CFLAGS) -c $(SERVER_DIR)server_options.cpp -o $(SERVER_DIR)server_options.o

$(UTILS_DIR)connection.o:  $(UTILS_DIR)connection.cpp $(UTILS_DIR)connection.hpp
	$(CC) $(CFLAGS) -c $(UTILS_DIR)connection.cpp -o $(UTILS_DIR)connection.o

.PHONY: clean 
clean:
	$(RM) -f  $(SERVER_DIR)*.o $(SERVER_DIR)*~ $(CLIENT_DIR)*.o $(CLIENT_DIR)*~ $(UTILS_DIR)*.o $(UTILS_DIR)*~

.PHONY: check-syntax
check-syntax:
	$(CC) $(CFLAGS) -fsyntax-only $(SOURCES)

.PHONY: deploy
deploy: $(SERVER_DIR)dataServer
	cd server && ./dataServer -p $(PORT) -s 4 -q 6

.PHONY: connect
connect: $(CLIENT_DIR)remoteClient
	./$(CLIENT_DIR)remoteClient -i 127.0.0.1 -p $(PORT) -d test

.PHONY: connect1
connect1: $(CLIENT_DIR)remoteClient
	./$(CLIENT_DIR)remoteClient -i 127.0.0.1 -p $(PORT) -d /home/anantoni/ta/my_syspro/tcp-client-server/test
