
typedef int (*msg_function_f)(void *, const char*, int);

ssize_t SendMessage(int socket_id, const char* text);
ssize_t RecieveMessage(int socket_id, void * ctx, msg_function_f);