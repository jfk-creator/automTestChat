#include "test01.h"

#include <assert.h>
#include <mqueue.h>
#include <unistd.h>
#define MAX_MSGS 10
#define MAX_MSG_SIZE 512
#define MSG_PRIO 1

void establishQueue(ClientArgs* clientArgs) {
  mqd_t mqCMDS;
  mqd_t mqREPORTS;
  struct mq_attr attr;

  attr.mq_flags = 0;
  attr.mq_maxmsg = MAX_MSGS;
  attr.mq_msgsize = MAX_MSG_SIZE;
  attr.mq_curmsgs = 0;

  char buffer[32 + 2];
  snprintf(buffer, sizeof(buffer), "/%s", clientArgs->name);
  mqCMDS = mq_open(buffer, O_CREAT | O_WRONLY, 0644, &attr);
  if (mqCMDS == (mqd_t)-1) {
    errorPrint("CMDS Mqueue not opened.");
    exit(EXIT_FAILURE);
  }

  clientArgs->mqCMDS = mqCMDS;

  memset(buffer, 0, sizeof(buffer));
  snprintf(buffer, sizeof(buffer), "/R%s", clientArgs->name);
  mqREPORTS = mq_open(buffer, O_CREAT | O_RDONLY, 0644, &attr);
  if (mqREPORTS == (mqd_t)-1) {
    errorPrint("RERPORTS Mqueue not opened.");
    exit(EXIT_FAILURE);
  }

  clientArgs->mqREPORTS = mqREPORTS;
}

void unlinkQueue(ClientArgs* clientArgs) {
  char buffer[32 + 2];
  snprintf(buffer, sizeof(buffer), "/%s", clientArgs->name);
  if (mq_unlink(buffer) == -1) {
    perror("mq_unlink failed");
    errorPrint("Failed to unlink queue %s.", buffer);
  }
  memset(buffer, 0, sizeof(buffer));
  snprintf(buffer, sizeof(buffer), "/R%s", clientArgs->name);
  if (mq_unlink(buffer) == -1) {
    perror("mq_unlink failed");
    errorPrint("Failed to unlink queue %s.", buffer);
  }
}

void receiveReport(ClientArgs* clientArgs) {
  char buffer[MAX_MSG_SIZE] = {0};
  unsigned int prio;
  ssize_t bytes_read =
      mq_receive(clientArgs->mqREPORTS, buffer, MAX_MSG_SIZE, &prio);
  if (bytes_read == -1) {
    errorPrint("receiv failed.");
    exit(EXIT_FAILURE);
  }
  buffer[bytes_read] = '\0';
  debugPrint("test01: %s received from %s", buffer, clientArgs->name);
}

void sendMessage(const char* cmd, ClientArgs* clientArgs) {
  if (mq_send(clientArgs->mqCMDS, cmd, strlen(cmd) + 1, MSG_PRIO) == -1) {
    errorPrint("mq_send failed");
  }
  debugPrint("Dispatcher Cmd: %s an %d", cmd, clientArgs->mqCMDS);
  receiveReport(clientArgs);
}

ClientArgs* buildClient(char* name) {
  ClientArgs* clientArgs = malloc(sizeof(ClientArgs));
  if (clientArgs == NULL) {
    errorPrint("clientArgs not allocated");
  }
  strncpy(clientArgs->name, name, sizeof(clientArgs->name));
  clientArgs->mqCMDS = (mqd_t)0;
  clientArgs->mqREPORTS = (mqd_t)0;

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
  establishQueue(clientArgs);
  receiveReport(clientArgs);
  unlinkQueue(clientArgs);
  assert(clientArgs != NULL);
  return clientArgs;
}

void deconstuctClient(ClientArgs* clientArgs) {
  close(clientArgs->socket);
  free(clientArgs);
}

bool runTest_01() {
  ClientArgs* frederike = buildClient("Frederike");
  ClientArgs* leon = buildClient("Leon");

  sendMessage("CONNECT", frederike);
  sendMessage("CONNECT", leon);

  sendMessage("LOGIN", frederike);
  sendMessage("LOGIN", leon);

  sendMessage("QUIT", frederike);
  sendMessage("QUIT", leon);

  deconstuctClient(frederike);
  deconstuctClient(leon);

  int8_t count = 63;
  ClientArgs* userArray[count];
  for (int8_t i = 0; i < count; i++) {
    char name[32];
    snprintf(name, 32, "user_%d", i);
    userArray[i] = buildClient(name);
  }
  for (int8_t i = 0; i < count; i++) {
    sendMessage("CONNECT", userArray[i]);
    sendMessage("LOGIN", userArray[i]);
  }
  for (int8_t i = 0; i < count; i++) {
    sendMessage("QUIT", userArray[i]);
    deconstuctClient(userArray[i]);
  }

  return true;
}
