//#define _DEFAULT_SOURCE //hmm, what does this do? i need it?

#include <sys/types.h>
#include <sys/socket.h>
#include <memory.h>
#include <stdio.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include "message.h"

#define PORT_NUMBER 4242

void signal_handler(int sn) 
{
    printf("Received signal %d\n", sn);
    while(1)
    {
        int st;
        int pid = waitpid(-1, &st, WNOHANG);
        if(pid == 0)
        {
            break; //none exites
        }
        if(pid < 0)
        {
            perror("Waitpid returned -1:");
            break;
        }
        printf("Child process %d terminated with %d\n", pid, WEXITSTATUS(st));
    }
}

/// WOW! VS Code can generate Doxigen comments!!!!
/// @brief process message, append message to context
/// @param ptr -- context used
/// @param text -- incoming msg
/// @param length size
/// @return 1 -- ok, -1 -- error
int server_message_handler(void* ptr, const char* text, int length) {

    my_buffer_t* ctx = (my_buffer_t*)ptr;
    // append text to ctx.data

    if(length < (ctx->available - ctx->len))
    {
        memcpy(ctx->data + ctx->len, text, length); // what? memcpy has no specific errors?!
        ctx->len += length;
    }
    else
    {
        //reallocate alittle more
        ctx->data = realloc(ctx->data, ctx->available + length * 2);
        if(ctx->data == NULL)
        {
            printf("Error append message to context");
            return -1;
        } 
        memcpy(ctx->data + ctx->len, text, length);
        ctx->len += length;
        ctx->available += (length * 2);
        printf("Buffer reallocation. New avaible size is %ld bytes", ctx->available );
    }
    printf ("Received data from Client %d bytes, message: %.*s\n", length, length, text);
    return 1;
}

int main(int argc, char * argv[])
{
    signal(SIGCHLD, signal_handler);
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

            // allocate/init
            // int res;
            // do
            // deallocate
            // exit(res)

            const size_t init_buf_size = 0; // I can init it by some other value. but I want to see realloc effect
            my_buffer_t * buf = ConstructBuffer(init_buf_size);
            if(buf == NULL )
            {
                //perror ("Error in recv():");
                printf("Error constructing buffer");
                //close listening socket?
                exit(EXIT_FAILURE);
            }

            // Receive messages from the client
            result = RecieveMessage(client_sckt, buf, server_message_handler);
            if(result < 0)
            {
                perror ("Error in recv():");
                //close listening socket?
                DestructBuffer(buf);
                exit(EXIT_FAILURE);
            }

            printf ("Received data from Client %.*s\n", result, buf->data);
            const char* pong = "pong";
            result =  SendMessage(client_sckt, pong);
            if( result < 0 )
            {
                perror ("Error in recv():");
                //close listening socket?
                DestructBuffer(buf);
                exit(EXIT_FAILURE);
            }
            printf("Sent to Client %s\n", pong);
            shutdown(client_sckt, SHUT_RDWR);
            DestructBuffer(buf);
            exit(EXIT_SUCCESS);
        } 
        else if(pid == -1)
        {
            perror ("Error fork():");
            close(client_sckt);
            abort();
            //continue; // or exit?
        }
        else
        {
            //it is parent process. what to do?!
        }
    }
    close(listen_sckt);
    exit(EXIT_SUCCESS);
}