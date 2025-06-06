#ifndef UTILS_H
#define UTILS_H

#include <stdarg.h>  // Required for va_list

typedef enum {
    RUN_ALL_TEST,
    TEST01,
    TEST02,
    TEST03,
    TEST04,
    TEST05,
} CurrentTestId;

// Function to initialize utility functions (e.g., set program name)
void utilInit(const char *argv0);

// Normal print function (to stdout)
void normalPrint(const char *fmt, ...);

// Debug print function (to stdout, yellow text)
void debugPrint(const char *fmt, ...);

// Error print function (to stderr, red text)
void errorPrint(const char *fmt, ...);

int readArgs(int argc, char **argv);

#endif  // UTILS_H