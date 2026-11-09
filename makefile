all:
	gcc server.c message.c -o server -fsanitize=address -g -Wall
	gcc client.c message.c -o client -fsanitize=address -g -Wall

client:
	gcc client.c message.c -o client

server: 
	gcc server.c message.c -o server

clean:
	rm -rf *.o client server