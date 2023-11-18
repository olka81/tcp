all:
	gcc -fsanitize=memory server.c message.c -o server
	gcc client.c message.c -o client

client:
	gcc client.c message.c -o client

server: 
	gcc server.c message.c -o server

clean:
	rm -rf *.o client server