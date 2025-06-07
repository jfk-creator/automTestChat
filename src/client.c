#include "client.h"

#include <arpa/inet.h>
#include <mqueue.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#define MAX_MSG_SIZE 512
#define STANDARD_PORT 8111
#define BUFFER_SIZE 1024

static ClientArgs* clientArgs;

typedef enum {
    CONNECT,
    QUIT,
    SEND,
    INVALID
} COMMANDS;

COMMANDS parseCmd(char* mqMsg) {
    debugPrint("%s received: %s", clientArgs->name, mqMsg);

    if (strcmp("QUIT", mqMsg) == 0) {
        return QUIT;
    }
    if (strcmp("CONNECT", mqMsg) == 0) {
        return CONNECT;
    }
    return INVALID;
}

void handleQuit(mqd_t mq) {
    debugPrint("Client: %s shutsdown.", clientArgs->name);

    if (mq_close(mq) == -1) {
        errorPrint("coudn't close mqeueu.");
        exit(EXIT_FAILURE);
    }
    pthread_exit(NULL);
}

void handleConnect(int16_t port, mqd_t mq) {
    debugPrint("Client: %s connectiong to 127.0.0.1:%d", clientArgs->name, port);
    struct sockaddr_in server_addr;
    // char buffer[BUFFER_SIZE] = {0};

    clientArgs->socket = socket(AF_INET, SOCK_STREAM, 0);
    if (clientArgs->socket == -1) {
        errorPrint("Error creating socket");
        exit(EXIT_FAILURE);
    }

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(STANDARD_PORT);

    if (inet_pton(AF_INET, "127.0.0.1", &server_addr.sin_addr) <= 0) {
        errorPrint("Invalid address/ Address not supported");
        close(clientArgs->socket);
        exit(EXIT_FAILURE);
    }

    if (connect(clientArgs->socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
        errorPrint("Error connecting to server");
        close(clientArgs->socket);
        exit(EXIT_FAILURE);
    }
    normalPrint("Successfully connected to the server.\n");

    char mq_buffer[MAX_MSG_SIZE];
    snprintf(mq_buffer, MAX_MSG_SIZE, "OK");
    if (mq_send(mq, mq_buffer, strlen(mq_buffer) + 1, 1) == -1) {
        errorPrint("mq_send failed");
    }
    debugPrint("Dispatcher: send message");
}

void* client(void* arg) {
    clientArgs = (ClientArgs*)arg;
    debugPrint("starting client: %s", clientArgs->name);
    // establish mqueue
    mqd_t mq = mq_open(clientArgs->qName, O_RDWR);
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

        switch (parseCmd(buffer)) {
            case CONNECT:
                handleConnect(STANDARD_PORT, mq);
                break;
            case QUIT:

                handleQuit(mq);
                break;
            case INVALID:
                errorPrint("Mq received an invalid command.");
                break;

            default:
                errorPrint("Hier hab ich mein Auto nicht geparkt.");
                break;
        }
    }
    if (mq_close(mq) == -1) {
        errorPrint("coudn't close mqeueu.");
        exit(EXIT_FAILURE);
    }

    pthread_exit(NULL);
}