#include "client.h"

#include <mqueue.h>
#include <stdlib.h>
#include <string.h>

#define MAX_MSG_SIZE 512

void* client(void* arg) {
    ClientArgs* clientArgs = (ClientArgs*)arg;
    debugPrint("starting client: %s", clientArgs->name);
    // establish mqueue
    mqd_t mq = mq_open(clientArgs->qName, O_RDONLY);
    if (mq == (mqd_t)-1) {
        errorPrint("mq not openend.");
        exit(EXIT_FAILURE);
    }
    debugPrint("client %s opened Mqeueu", clientArgs->name);

    while (1) {
        unsigned int prio;
        char buffer[MAX_MSG_SIZE];
        ssize_t bytes_read = mq_receive(mq, buffer, MAX_MSG_SIZE, &prio);
        if (bytes_read == -1) {
            errorPrint("receiv failed.");
            exit(EXIT_FAILURE);
        }
        buffer[bytes_read] = '\0';
        debugPrint("%s received: %s", clientArgs->name, buffer);
        if (strcmp("QUIT", buffer) == 0) {
            debugPrint("Client: %s shutsdown.", clientArgs->name);
        }
    }
    if (mq_close(mq) == -1) {
        errorPrint("coudn't close mqeueu.");
        exit(EXIT_FAILURE);
    }
    // connect to 127.0.0.1:8111 on default
    // send login package
    pthread_exit(NULL);
}