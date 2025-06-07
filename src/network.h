
#include <stdint.h>

#define MAGIC_NUMBER 0x0badf00d
#define VERSION 0
// LoginRequest Type 0
typedef struct {
  uint32_t magic;
  uint8_t version;
  char name[31];  // Not Null terminated
} __attribute__((packed)) LGQ;

typedef struct {
  uint8_t type;
  uint16_t length;
  LGQ logingRequest;
} __attribute__((packed)) paket_t;
