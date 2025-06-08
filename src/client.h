#ifndef CLIENT_H
#define CLIENT_H

#include <mqueue.h>
#include <pthread.h>
#include <stdio.h>

#include "utils.h"

typedef struct {
  char name[32];
  char qName[32 + 1];
  int socket;
  pthread_t threadId;
  mqd_t mqReceiveCMDS;
  mqd_t mqSendReport;
} ClientArgs;

void* client(void* arg);

#endif