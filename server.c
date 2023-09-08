#define _DEFAULT_SOURCE

#include <sys/types.h>
#include <sys/socket.h>
#include <memory.h>
#include <stdio.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define PORT_NUMBER 4242

int main(int argc, char * argv[])
{
    int listen_sckt = socket(AF_INET, SOCK_STREAM, 0);
    if(listen_sckt  < 0 )
    {
        //perror
        return listen_sckt;
    }
    struct sockaddr_in peer;
    memset(&peer, 0, sizeof(peer));

    peer.sin_family = AF_INET;
    peer.sin_port = htons(PORT_NUMBER);
    peer.sin_addr.s_addr = INADDR_ANY;
    int result = bind(listen_sckt, (struct sockaddr *)&peer, sizeof(peer)); // I still don't understand, how we can cast sockaddr_in to sockaddr
    if(result  < 0)
    {
        //perror
        //kill socket?
        return result;
    }
    printf("I'll be waiting\r\n");
    result = listen(listen_sckt, SOMAXCONN);
    if(result != 0)
    {
        //perror
        //kill socket?
        //kill connection?
        return result;
    }
    int client_sckt = accept(listen_sckt, NULL, NULL);
    if( client_sckt < 0 )
    {
        //perror
        //kill socket?
        //kill connection?
        return result;
    }
    shutdown(listen_sckt, 1);
    char buffer[256];
    memset(buffer, 0, 256);
    result = recv(client_sckt, buffer, 255, 0);
    if(result < 0)
    {
        //perror
        //kill socket?
        //kill connection?
        return result;
    }
    printf("I recieved %s\r\n", buffer);
    char pong[] = "pong";
    result = send( client_sckt, pong, strlen(pong), 0);
    if( result <= 0 )
    {
        //error send
        //kill socket?
        //kill connect?
        return result;
    }
    printf("I sent %s\r\n", pong);
    shutdown(client_sckt, 0);




}
