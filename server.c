#define _DEFAULT_SOURCE

#include <sys/types.h>
#include <sys/socket.h>
#include <memory.h>
#include <stdio.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

#define PORT_NUMBER 4242

int main(int argc, char * argv[])
{
    int listen_sckt = socket(AF_INET, SOCK_STREAM, 0);
    if(listen_sckt  < 0 )
    {
        perror("Error socket(): ");
        return listen_sckt;
    }
    printf("Server Socket is created.\n");
    struct sockaddr_in peer;
    memset(&peer, 0, sizeof(peer));

    peer.sin_family = AF_INET;
    peer.sin_port = htons(PORT_NUMBER);
    peer.sin_addr.s_addr = INADDR_ANY;
    int result = bind(listen_sckt, (struct sockaddr *)&peer, sizeof(peer)); // I still don't understand, how we can cast sockaddr_in to sockaddr
    if(result  < 0)
    {
        perror("Error bind(): ");
        return result;
    }
    printf("Bind listening socket\n");
    result = listen(listen_sckt, SOMAXCONN); //we should th
    if(result != 0)
    {
        perror("Error listen(): ");
        return result;
    }
    printf("Listening...\n");
    char buffer[256];
    while (1) 
    {
        struct sockaddr_in clientAddr;
        socklen_t addrSize = sizeof (clientAddr);
        int client_sckt = accept(listen_sckt, (struct sockaddr* )&clientAddr, &addrSize);
        if( client_sckt < 0 )
        {
            perror("Error accept(): ");
            return result;
        }
        printf("Accepted connection from Client %s:%d\n", inet_ntoa(clientAddr.sin_addr), ntohs(clientAddr.sin_port));
        pid_t pid = fork ();
        if (pid == 0) 
        {
            // Receive messages from the client
            while (1) 
            {
                memset (buffer, 0, sizeof(buffer));
                if (result = recv (client_sckt , buffer, sizeof (buffer), 0) < 0) {
                    perror ("Error in recv():");
                    return result;
                }

                printf ("Received data from Client %s\n", buffer);
                const char* pong = "pong";
                result = send( client_sckt, pong, strlen(pong), 0);
                if( result <= 0 )
                {
                    return result;
                }
                printf("Sent to Client %s\n", pong);        
                shutdown(client_sckt, SHUT_RDWR);
            }
        } 
        else 
        {
            //it is parent process. what to do?!
        }
    }
}
