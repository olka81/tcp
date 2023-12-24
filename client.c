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

#include "message.h"

#define PORT_NUMBER 4242

/// @brief let it be stupid/ I have already smart handler at server side. no context. just prinft
/// @param ptr -- not used
/// @param text -- incoming msg
/// @param length size
/// @return 1 -- ok, -1 -- error
int client_message_handler(void* ptr, const char* text, int length) 
{
    printf("I recieved %*.s ", length, text);
    return 1;
}

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
    //const char* ping = "It's my first try to encode message";
    const char* ping = "123456789012345678901234567890abcdefg";
    result = SendMessage(my_sckt, ping);

    if( result <= 0 )
    {
        perror("Error send(): ");
        close(my_sckt);
        exit(EXIT_FAILURE);
    }
    printf("I sent %s\r\n", ping);
    //I still don't understand shoud I call shutdown with SHUT_WR in this case or not
    //shutdown(my_sckt, SHUT_WR);

    char* empty_context = NULL;
    result = RecieveMessage(my_sckt, empty_context, client_message_handler);
    //recv(my_sckt, recieveBuf, 255, 0);
    if(result < 0)
    {
        perror("Error recv(): ");
    }
    shutdown(my_sckt, SHUT_RDWR); 
    exit(EXIT_SUCCESS);
}
