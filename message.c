
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include "message.h"

#define MAX_PAYLAOD 30
#define HEADER_SIZE 30

/**
 * Sends the message through the specified socket, chopping it into multiple payloads if necessary.
 * @param socket_id socket to send through
 * @param text payload
 * @return the number of payload bytes that was actually sent, or a negative value if send failed outright.
 */
ssize_t SendOnePack(int socket_id, uint8_t tag, uint8_t length, char* text_with_oofset)
{
    char buffer[MAX_PAYLAOD + HEADER_SIZE];
    buffer[0] == tag;
    buffer[1] = length;
    strncpy(buffer + HEADER_SIZE, text_with_oofset, length);
    return send( socket_id, buffer, length + HEADER_SIZE, 0);
}

ssize_t SendMessage(int socket_id, const char* text)
{
    //TLV:
    // 1 byte Tag:
    //  0 -- text, utf-8, short message, le MAX_PAYLAOD bytes //for the test it'll limited by 16 bytes
    //  1 -- text, utf-8, part of long message
    //  4 -- eof, for long messages only
    // 1 byte Length
    // <MAX_PAYLAOD byte text

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
    if(send(socket_id, eof, 2, 0 ) < 0)
    {
        return -1;
    }
    else 
    {
        return sent;
    }    
}

size_t RecieveMessage(int socket_id, void * ctx, msg_function_f msg_function)
{
    uint8_t header[2];
    uint8_t length;
    size_t recieved = 0;
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
            msg_function(ctx, buffer, recieved);
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
                msg_function(ctx, buffer, recieved);
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