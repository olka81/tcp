//#define _DEFAULT_SOURCE //hmm, what does this do? i need it?

#include <sys/types.h>
#include <sys/socket.h>
#include <memory.h>
#include <stdio.h>
#include <stdlib.h>
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
        exit(EXIT_FAILURE);
    }
    printf("Server Socket is created.\n");
    struct sockaddr_in peer;
    memset(&peer, 0, sizeof(peer));

    peer.sin_family = AF_INET;
    peer.sin_port = htons(PORT_NUMBER);
    peer.sin_addr.s_addr = INADDR_ANY;

    int yes = 1;
    if (setsockopt(listen_sckt, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1) {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }

    int result = bind(listen_sckt, (struct sockaddr *)&peer, sizeof(peer)); // I still don't understand, how we can cast sockaddr_in to sockaddr

    if(result  < 0)
    {
        perror("Error bind(): ");
        close(listen_sckt); 
        exit(EXIT_FAILURE);
    }
    printf("Bind listening socket\n");
    result = listen(listen_sckt, 5); //let it be the magic number 5. 
    if(result != 0)
    {
        perror("Error listen(): ");
        close(listen_sckt); 
        exit(EXIT_FAILURE);
    }
    printf("Listening...\n");

    char buffer[256];
    int max_buf = sizeof(buffer);

    while (1) 
    {
        struct sockaddr_in clientAddr;
        socklen_t addrSize = sizeof (clientAddr);
        int client_sckt = accept(listen_sckt, (struct sockaddr* )&clientAddr, &addrSize);
        if( client_sckt < 0 )
        {
            perror("Error accept(): ");
            continue;
        }
        printf("Accepted connection from Client %s:%d\n", inet_ntoa(clientAddr.sin_addr), ntohs(clientAddr.sin_port));
        pid_t pid = fork ();
        if (pid == 0) //child process
        {
            // Receive messages from the client
            if ((result = recv (client_sckt , buffer, max_buf, 0)) < 0) {
                perror ("Error in recv():");
                //close listening socket?
                exit(EXIT_FAILURE);
            }

            // buffer[result] = 0;

            printf ("Received data from Client %.*s\n", result, buffer);
            const char* pong = "pong";
            result = send( client_sckt, pong, strlen(pong), 0);
            if( result < 0 )
            {
                perror ("Error in recv():");
                //close listening socket?
                exit(EXIT_FAILURE);
            }
            printf("Sent to Client %s\n", pong);
            shutdown(client_sckt, SHUT_RDWR);
            exit(EXIT_SUCCESS);
        } 
        else if(pid == -1)
        {
            perror ("Error fork():");
            close(client_sckt);
            continue; // or exit?
        }
        else
        {
            //it is parent process. what to do?!
        }
    }
    close(listen_sckt);
    exit(EXIT_SUCCESS);
}
