#include "client.h"

void* client(void* arg) {
    ClientArgs* clientArgs = (ClientArgs*)arg;
    debugPrint("starting client: %s", clientArgs->name);
    pthread_exit(NULL);
}