all:
	gcc server.c message.c -o server -fsanitize=address -static-libasan -g -Wall
	gcc client.c message.c -o client -fsanitize=address -static-libasan -g -Wall

client:
	gcc client.c message.c -o client

server: 
	gcc server.c message.c -o server

clean:
	rm -rf *.o client server