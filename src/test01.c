#include "test01.h"

#include <assert.h>
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
  debugPrint("Mqueue openend: %d", mq_handle);
  return mq_handle;
}

void routine01(ClientArgs* clientArgs) {
  char buffer[MAX_MSG_SIZE] = {0};

  snprintf(buffer, MAX_MSG_SIZE, "CONNECT");
  if (mq_send(clientArgs->mq, buffer, strlen(buffer) + 1, MSG_PRIO) == -1) {
    errorPrint("mq_send failed");
  }
  debugPrint("Dispatcher: send message");

  sleep(1);

  memset(&buffer, 0, sizeof(buffer));
  unsigned int prio;
  ssize_t bytes_read = mq_receive(clientArgs->mq, buffer, MAX_MSG_SIZE, &prio);
  if (bytes_read == -1) {
    errorPrint("receiv failed.");
    exit(EXIT_FAILURE);
  }
  buffer[bytes_read] = '\0';
  debugPrint("test01: %s received from %s", buffer, clientArgs->name);

  sleep(1);

  snprintf(buffer, MAX_MSG_SIZE, "LOGIN");
  if (mq_send(clientArgs->mq, buffer, strlen(buffer) + 1, MSG_PRIO) == -1) {
    errorPrint("mq_send failed");
  }
  sleep(1);

  debugPrint("Dispatcher: send message");

  snprintf(buffer, MAX_MSG_SIZE, "QUIT");
  if (mq_send(clientArgs->mq, buffer, strlen(buffer) + 1, MSG_PRIO) == -1) {
    errorPrint("mq_send failed");
  }
  debugPrint("Dispatcher: send message");

  sleep(3);
}

ClientArgs* buildClient(char* name) {
  ClientArgs* clientArgs = malloc(sizeof(ClientArgs));
  if (clientArgs == NULL) {
    errorPrint("clientArgs not allocated");
  }
  strncpy(clientArgs->name, name, sizeof(clientArgs->name));
  snprintf(clientArgs->qName, sizeof(clientArgs->qName), "/%s",
           clientArgs->name);
  clientArgs->qName[32] = '\0';

  clientArgs->mq = establishQueue(clientArgs->qName);
  int result =
      pthread_create(&clientArgs->threadId, NULL, client, (void*)clientArgs);
  if (result != 0) {
    errorPrint("Failed to create client thread");
    free(clientArgs);
    exit(EXIT_FAILURE);
  } else {
    debugPrint("Client thread created successfully with ID: %lu",
               (unsigned long)clientArgs->threadId);
  }
  assert(clientArgs != NULL);
  return clientArgs;
}

void deconstuctClient(ClientArgs* clientArgs) {
  if (mq_close(clientArgs->mq) == -1) {
    errorPrint("coudn't close mqeueu.");
    exit(EXIT_FAILURE);
  }
  close(clientArgs->socket);
  free(clientArgs);
}

bool runTest_01() {
  normalPrint("Starting Test01");
  ClientArgs* frederike = buildClient("Frederike");

  routine01(frederike);

  deconstuctClient(frederike);

  return true;
}
