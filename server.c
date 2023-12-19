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

/**
 * Struct to store context
 *  @note data
 *  @note len used length
 *  @note available allocated memory amount
 */
typedef struct my_buffer {
    char * data;
    size_t len;
    size_t available;
} my_buffer_t;

// I want to emulate ctor and dtor

/**
 * Constructor for my struct
 *  @param init_size initial available allocated memory amount
 *  @return result ptr or NULL
 */
my_buffer_t* ConstructBuffer(size_t init_size)
{
    my_buffer_t* ptr = (my_buffer_t*)malloc(sizeof(my_buffer_t));
    if(ptr == NULL)
    {
        return NULL;
    }
    ptr->len = 0;
    ptr->data = (char *)malloc(init_size);
    if(ptr->data == NULL)
    {
        free(ptr);
        return NULL;
    }
    ptr->available = init_size;
    return ptr;
}


void DestructBuffer(my_buffer_t* ptr)
{
    free(ptr->data);
    free(ptr);
}

/// WOW! VS Code can generate Doxigen comments!!!!
/// Append message to context
/// @param ptr context used
/// @param text incoming msg
/// @param length  size
/// @return lenght or -1
ssize_t AppendBuffer(my_buffer_t* ptr, char* text, size_t length)
{
    if(length < (ptr->available - ptr->len))
    {
        memcpy(ptr->data + ptr->len, text, length); // what? memcpy has no specific errors?!
        ptr->len += length;
        //append
    }
    else
    {
        //reallocate alittle more
        ptr->data = realloc(ptr->data, ptr->available + length * 2);
        if(ptr->data == NULL)
            return -1;
        memcpy(ptr->data + ptr->len, text, length);
        ptr->len += length;
        ptr->available += (length * 2);
        //append
    }
    return length; //if ut's ok
}

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

int message_handler(void* ptr, const char* text, int length) {

    my_buffer_t* ctx = (my_buffer_t*)ptr;
    // append text to ctx.data

    printf ("Received data from Client %d bytes, message: %.*s\n", length, length, text);
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

            // allocate/init
            // int res;
            // do
            // deallocate
            // exit(res)

            my_buffer_t buf = {0};

            // Receive messages from the client
            //if ((result = recv (client_sckt , buffer, max_buf, 0)) < 0) {
            if((result = RecieveMessage(client_sckt, &buf, message_handler) < 0))
            {
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