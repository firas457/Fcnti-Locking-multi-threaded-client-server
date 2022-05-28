
all: client server
	
client: client.o
	g++ -o client client.o

server: server.o 
	g++ -pthread  -o  server server.o  -lrt


client.o: client.cpp
	g++ -c client.cpp

server.o: server.cpp
	g++ -pthread -c server.cpp 


.PHONY: clean all

clean:
	rm -f *.o server client