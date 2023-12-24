
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include "message.h"

#define MAX_PAYLAOD 30
#define HEADER_SIZE 2
    //TLV:
    // 1 byte Tag:
    //  0 -- text, utf-8, short message, le MAX_PAYLAOD bytes //for the test it'll limited by 16 bytes
    //  1 -- text, utf-8, part of long message
    //  4 -- eof, for long messages only
    // 1 byte Length
    // <MAX_PAYLAOD byte text


/// @brief sends one payload <=MAX_PAYLAOD with specified header
/// @param socket_id socket to send through
/// @param tag 
/// @param length 
/// @param text_with_oofset 
/// @return actually sent or a negative value
ssize_t SendOnePack(int socket_id, uint8_t tag, uint8_t length, const char* text_with_ofset)
{
    char buffer[MAX_PAYLAOD + HEADER_SIZE];
    buffer[0] = tag;
    buffer[1] = length;
    strncpy(buffer + HEADER_SIZE, text_with_ofset, length);
    return send( socket_id, buffer, length + HEADER_SIZE, 0);
}

/**
 * Sends the message through the specified socket, chopping it into multiple payloads if necessary.
 * @param socket_id socket to send through
 * @param text payload
 * @return the number of payload bytes that was actually sent, or a negative value if send failed outright.
 */
ssize_t SendMessage(int socket_id, const char* text)
{
    if (!text) {
        return -1;
    }

    size_t length = strlen(text);
    uint8_t eof[HEADER_SIZE] = {4, 0};;
    ssize_t sent = 0;
    if(length <= MAX_PAYLAOD)
    {
        ssize_t cur_sent = SendOnePack(socket_id, 0, (uint8_t)length, text);
        return (cur_sent < 0) ? cur_sent : cur_sent - HEADER_SIZE;
        //short message, no eof must be sent
    }

    while( sent < length )
    {
        ssize_t cur_sent = SendOnePack(socket_id, 1, (uint8_t)(length - sent), text + sent);
        if(cur_sent < 0)
        {
            return cur_sent;
        }
        sent += (cur_sent - 2);            
    }
    //eof, final part of long message
    if(send(socket_id, eof, 2, 0 ) < 0)
    {
        return -1;
    }
    else 
    {
        return sent;
    }    
}

/// @brief resieve TLV message and provide it to msg_function
/// @param socket_id -- listening socket
/// @param ctx -- some context
/// @param msg_function -- prividing function. works with ctx. shoul return the same lenght or error
/// @return recieved payload amount, without headers.
///         or errors: -1 recv error, -2 msg_function error
ssize_t RecieveMessage(int socket_id, void * ctx, msg_function_f msg_function)
{
    uint8_t header[2];
    uint8_t length;
    ssize_t recieved = 0;
    char buffer[MAX_PAYLAOD];
    memset(buffer, 0, MAX_PAYLAOD);
    if(recv (socket_id , header, 2, 0) < 0)
    {
        return -1;
    }
    printf ("Received TL %d%d\n", header[0], header[1]);
    if(header[0] == 0) //it's a short msg
    {
        length = header[1];
        if(length > 0)
        {
            recieved = recv (socket_id, buffer, length, 0);
            if(msg_function(ctx, buffer, length) != 1)
            {
                return -2;
            }
            memset(buffer, 0, MAX_PAYLAOD);
        }
        return recieved; //cmp length and recieved?
    }
    else
    {
        while(header[0] == 1)
        {
            length = header[1];
            if(length > 0)
            {
                memset(buffer, 0, MAX_PAYLAOD);
                recieved += recv (socket_id, buffer, length, 0);
                if(msg_function(ctx, buffer, length) != 1)
                {
                    return -2;
                }                
            }
            if(recv (socket_id , header, 2, 0) < 0)
            {
                return -1;
            }
            printf ("Received TL %d%d\n", header[0], header[1]);
        }
        return recieved; //we'll get here in the end of long message, header[0] is 4, header[1] is 0
    }
}

/**
 * Constructor for my struct
 *  @param init_size initial available allocated memory amount
 *  @return result ptr or NULL
 */
my_buffer_t* ConstructBuffer(const size_t init_size)
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