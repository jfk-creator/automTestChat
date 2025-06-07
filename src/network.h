
#ifndef NETWORK_H
#define NETWORK_H

#include <errno.h>
#include <stdint.h>
#include <unistd.h>

#include "utils.h"

#define MAGIC_NUMBER 0x0badf00d
#define VERSION 0
// LoginRequest Type 0
typedef struct {
  uint32_t magic;
  uint8_t version;
  char name[31];  // Not Null terminated
} __attribute__((packed)) LGQ;

typedef struct {
  uint32_t magic;
  uint8_t code;
  char sName[31];
} __attribute__((packed)) LRE;

union Body {
  LGQ loginRequest;
  LRE loginResponse;
};

typedef struct {
  uint8_t type;
  uint16_t length;
} __attribute__((packed)) Head;

typedef struct {
  uint8_t type;
  uint16_t length;
  union Body body;
} __attribute__((packed)) paket_t;

ssize_t recv_all(int socket_fd, void *buffer, size_t len);
#endif