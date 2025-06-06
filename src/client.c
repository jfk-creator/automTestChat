#include "client.h"

void* client(void* arg) {
    ClientArgs* clientArgs = (ClientArgs*)arg;
    debugPrint("starting client: %s", clientArgs->name);
    // establish mqueue
    // connect to 127.0.0.1:8111 on default
    // send login package
    pthread_exit(NULL);
}