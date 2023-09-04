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
        //error init socket
        return my_sckt;
    }
    struct sockaddr_in peer;
    
    peer.sin_family = AF_INET;
    peer.sin_port = PORT_NUMBER;
    peer.sin_addr.s_addr = htonl(INADDR_ANY);
    int result = bind(my_sckt, (struct sockaddr *)&peer, sizeof(peer));
    if(result  < 0)
    {
        //error
        //kill socket&
        return result;
    }
    result = listen(my_sckt, 1);
    if(result != 0)
    {
        //error
        //kill socket?
        //kill connection?
        return result;
    }

    result = accept(my_sckt, NULL, NULL);
    if( result < 0 )
    {
        return result;
    }
    
}