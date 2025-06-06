#ifndef CLIENT_H
#define CLIENT_H

#include <pthread.h>
#include <stdio.h>

#include "utils.h"

typedef struct {
    char name[32];
} ClientArgs;
void* client(void* arg);

#endif