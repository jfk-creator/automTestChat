#ifndef CLIENT_H
#define CLIENT_H

#include <mqueue.h>
#include <pthread.h>
#include <stdio.h>

#include "utils.h"

typedef struct {
  char name[32];
  int socket;
  pthread_t threadId;
  mqd_t mqCMDS;
  mqd_t mqREPORTS;
} ClientArgs;

void* client(void* arg);

#endif