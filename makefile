all:
	gcc server.c -o server
	gcc client.c message.c -o client

client:
	gcc client.c message.c -o client

server: 
	gcc server.c -o server

clean:
	rm -rf *.o client server