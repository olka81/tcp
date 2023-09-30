//#define _DEFAULT_SOURCE

#include <sys/types.h>
#include <sys/socket.h>
#include <memory.h>
#include <stdio.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <arpa/inet.h>
//#include <errno.h>
#include <unistd.h>

#define PORT_NUMBER 4242

int main(int argc, char * argv[])
{
    int my_sckt = socket(AF_INET, SOCK_STREAM, 0);
    if(my_sckt  < 0 )
    {
        perror("Error socket(): ");
        exit(EXIT_FAILURE);
    }
    printf("Client socket created\n");
    struct sockaddr_in peer;

    peer.sin_family = AF_INET;
    peer.sin_port = htons(PORT_NUMBER);
    peer.sin_addr.s_addr = inet_addr("127.0.0.1");
    int result = connect(my_sckt, (const struct sockaddr*)&peer, sizeof(peer));
    if(result != 0)
    {
        perror("Error connect(): ");
        close(my_sckt);
        exit(EXIT_FAILURE);
    }
    printf("Connected to server\n");
    const char* ping = "ping";
    result = send( my_sckt, ping, strlen(ping), 0);
    if( result <= 0 )
    {
        perror("Error send(): ");
        close(my_sckt);
        exit(EXIT_FAILURE);
    }
    printf("I sent ping\r\n");
    //I still don't understand shoud I call shutdown with SHUT_WR in this case or not
    //shutdown(my_sckt, SHUT_WR);

    //well, I still have problems with implementation of a logical part: how should my server and client support handling messages. TLV or smth
    char recieveBuf[256];
    memset(recieveBuf, 0, 256);
    result = recv(my_sckt, recieveBuf, 255, 0);
    if(result < 0)
    {
        perror("Error recv(): ");
    }
    printf("I recieved %s\r\n", recieveBuf);
    shutdown(my_sckt, SHUT_RDWR); 
    exit(EXIT_SUCCESS);
}
