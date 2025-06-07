#ifndef UTILS_H
#define UTILS_H

#include <stdarg.h>  // Required for va_list
#include <stdbool.h>

typedef enum {
  RUN_ALL_TESTS,
  TEST01,
  TEST02,
  TEST03,
  TEST04,
  TEST05,
} CurrentTestId;

void setDebugOn();

void normalPrint(const char *fmt, ...);
void debugPrint(const char *fmt, ...);
void partSuccesPrint(const char *fmt, ...);
void testSuccesPrint(const char *fmt, ...);
void errorPrint(const char *fmt, ...);
int readArgs(int argc, char **argv);

#endif  // UTILS_H