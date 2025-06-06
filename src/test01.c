#include "test01.h"

#include <mqueue.h>
#include <unistd.h>
#define MAX_MSGS 10
#define MAX_MSG_SIZE 512
#define MSG_PRIO 1

mqd_t establishQueue(char* qName) {
    mqd_t mq_handle;
    struct mq_attr attr;

    attr.mq_flags = 0;
    attr.mq_maxmsg = MAX_MSGS;
    attr.mq_msgsize = MAX_MSG_SIZE;
    attr.mq_curmsgs = 0;

    mq_handle = mq_open(qName, O_CREAT | O_RDWR, 0644, &attr);

    if (mq_handle == (mqd_t)-1) {
        errorPrint("Mqueue not opened.");
        exit(EXIT_FAILURE);
    }
    debugPrint("Mqueue openend");
    return mq_handle;
}

bool runTest_01() {
    normalPrint("Starting Test01");
    pthread_t threadID;
    ClientArgs* clientArgs = malloc(sizeof(ClientArgs));
    if (clientArgs == NULL) {
        errorPrint("clientArgs not allocated");
    }
    strncpy(clientArgs->name, "Frederike", 32);
    strncpy(clientArgs->qName, "/frederike", 32);

    mqd_t mq_client = establishQueue(clientArgs->qName);
    char buffer[MAX_MSG_SIZE];
    int result = pthread_create(&threadID, NULL, client, (void*)clientArgs);
    if (result != 0) {
        errorPrint("Failed to create client thread");
        free(clientArgs);
        return false;
    } else {
        debugPrint("Client thread created successfully with ID: %lu", (unsigned long)threadID);
    }
    for (size_t i = 0; i < 10; i++) {
        if (i == 5) {
            snprintf(buffer, MAX_MSG_SIZE, "QUIT");
            if (mq_send(mq_client, buffer, strlen(buffer) + 1, MSG_PRIO) == -1) {
                errorPrint("mq_send failed");
            }
            debugPrint("Dispatcher: send message");
        }
        snprintf(buffer, MAX_MSG_SIZE, "This is what love feels like.");
        if (mq_send(mq_client, buffer, strlen(buffer) + 1, MSG_PRIO) == -1) {
            errorPrint("mq_send failed");
        }
        debugPrint("Dispatcher: send message");
    }

    sleep(3);

    return true;
}
