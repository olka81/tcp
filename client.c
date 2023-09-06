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
    int my_sckt = socket(AF_INET, SOCK_STREAM, 0);
    if(my_sckt  < 0 )
    {
        //error init socketprintf("I'm waiting\r\n");
        return my_sckt;
    }
    struct sockaddr_in peer;
    
    peer.sin_family = AF_INET;
    peer.sin_port = PORT_NUMBER;
    peer.sin_addr.s_addr = inet_addr("127.0.0.1");
    int result = connect(my_sckt, (const struct sockaddr*)&peer, sizeof(peer));
    if(result != 0)
    {
        //connect error
        //kill socket?
        return result;
    }
    char ping[] = "ping";
    result = send( my_sckt, ping, strlen(ping), 0);
    if( result <= 0 )
    {
        //error send
        //kill socket?
        //kill connect?
        return result;
    }
    printf("I sent ping\r\n");
    shutdown(my_sckt, SHUT_WR);

    char recieveBuf[256];
    memset(recieveBuf, 0, 256);
    result = recv(my_sckt, recieveBuf, 255, 0);
    if(result < 0)
        {
            //error
        }
    printf("I recieved %s\r\n", recieveBuf);
    shutdown(my_sckt, 0);
}