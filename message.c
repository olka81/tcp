
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include "message.h"

#define MAX_MESSAGE 30

size_t SendMessage(int socket_id, const char* text)
{
    //TLV:
    // 1 byte Tag:
    //  0 -- text, utf-8, short message, le MAX_MESSAGE bytes //for the test it'll limited by 16 bytes
    //  1 -- text, utf-8, part of long message
    //  4 -- eof, for long messages only
    // 1 byte Length
    // <MAX_MESSAGE byte text

    uint8_t tag; 
    size_t length = strlen(text); 
    char buffer[MAX_MESSAGE];
    char eof[2] = "40";
    memset(buffer, 0, MAX_MESSAGE);
    size_t sent = 0;
    while( sent < length )
    {
        if(length < MAX_MESSAGE) //it's either short message or end of long message
        {
            if(buffer[0] == 0) //it's a short message
            {
                buffer[0] == 0;
                buffer[1] = length;
                strncpy(buffer + 2, text, length);
                buffer[length + 2] = '\0';
                sent = send( socket_id, buffer, length + 2, 0);
                return sent - 2; //even if it's not ok, we know it on the calling side
            }
            else // (buffer[0] == 1)
            {
                buffer[1] = length;
                strncpy(buffer + 2, text, length);
                buffer[length + 2] = '\0';
                int cur_sent = send( socket_id, buffer + sent, length - sent + 2, 0);
                if(cur_sent <= 0)
                    return cur_sent;
                sent += (cur_sent - 2); //maybe I need some constant for tag+langth size
            }            
        }
        else
        {
            buffer[0] = 1;
            buffer[1] = MAX_MESSAGE;
            strncpy(buffer + 2, text, MAX_MESSAGE);
            buffer[MAX_MESSAGE + 2] = '\0';
            int cur_sent = send( socket_id, buffer, MAX_MESSAGE + 2, 0);
            if(cur_sent <= 0)
                return cur_sent;
            sent += (cur_sent - 2);
        }
    }
    if(send( socket_id, eof, 2, 0 ) < 0)
    {
        return -1;
    }
    else 
    {
        return sent;
    }    
}