
typedef int (*msg_function_f)(void *, const char*, int);

ssize_t SendMessage(int socket_id, const char* text);
ssize_t RecieveMessage(int socket_id, void * ctx, msg_function_f);

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
my_buffer_t* ConstructBuffer(const size_t init_size);
void DestructBuffer(my_buffer_t* ptr);