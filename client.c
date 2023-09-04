#include <sys/types.h>
#include <sys/socket.h>
#include <memory.h>
#include <stdio.h>
#include <netinet/in.h>

#define PORT_NUMBER 4242

int main(int argc, char * argv[])
{
    int my_sckt = socket(AF_INET, SOCK_STREAM, 0);
    if(my_sckt  < 0 )
    {
        //error init socket
        return my_sckt;
    }
    struct sockaddr_in  peer;

    peer.sin_family = AF_INET;
    peer.sin_port = PORT_NUMBER;
    peer.sin_addr.s_addr = inet_addr("127.0.0.1");
    int result = connect(my_sckt, &peer, sizeof(peer));
}