
size_t SendMessage(int socket_id, const char* text);
size_t RecieveMessage(int socket_id, char* text, int (*msgFunction) (const char*, int));