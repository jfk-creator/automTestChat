#include "utils.h"  // Include our own header first

#include <stdarg.h>  // For va_list, va_start, va_end
#include <stdio.h>  // For printf, fprintf, vprintf, vfprintf, stdout, stderr, etc.
#include <string.h>  // For strlen (though not used in this simplified version, good for common utilities)
#include <unistd.h>  // For isatty (to check if output is a terminal)

// ANSI escape codes for text styling
#define ANSI_COLOR_YELLOW "\x1b[33m"
#define ANSI_COLOR_RED "\x1b[31m"
#define ANSI_COLOR_GREEN "\x1b[32m"
#define ANSI_COLOR_RESET "\x1b[0m"
#define CHECK_HEAVY_MARK "\xE2\x9C\x85"
#define ICON_TROPHY "\xF0\x9F\x8F\x86"        // 🏆
#define ICON_GREEN_CIRCLE "\xF0\x9F\x9F\xA2"  // 🟢
#define ICON_RED_X "\xE2\x9D\x8C"             // ❌

static bool debugOn = false;

void setDebugOn() { debugOn = true; }

// Helper function to check if styling is supported (i.e., if it's a TTY)
static int is_terminal(FILE *file) { return isatty(fileno(file)); }

// Normal print function: prints to stdout
void normalPrint(const char *fmt, ...) {
  va_list args;
  va_start(args, fmt);

  vfprintf(stdout, fmt, args);
  fprintf(stdout, "\n");
  va_end(args);
}

// Debug print function: prints to stdout in yellow
void debugPrint(const char *fmt, ...) {
  if (debugOn) {
    va_list args;
    va_start(args, fmt);
    if (is_terminal(stdout)) {
      fprintf(stdout, ANSI_COLOR_YELLOW);
    }
    fprintf(stdout, "DEBUG: \t");

    vfprintf(stdout, fmt, args);
    if (is_terminal(stdout)) {
      fprintf(stdout, ANSI_COLOR_RESET);
    }
    fprintf(stdout, "\n");
    va_end(args);
  }
}

void partSuccesPrint(const char *fmt, ...) {
  va_list args;
  va_start(args, fmt);
  if (is_terminal(stdout)) {
    fprintf(stdout, ANSI_COLOR_GREEN ICON_GREEN_CIRCLE);
  }
  fprintf(stdout, " ");

  vfprintf(stdout, fmt, args);
  if (is_terminal(stdout)) {
    fprintf(stdout, ANSI_COLOR_RESET);
  }
  fprintf(stdout, "\n");
  va_end(args);
}

void testSuccesPrint(const char *fmt, ...) {
  va_list args;
  va_start(args, fmt);
  if (is_terminal(stdout)) {
    fprintf(stdout, ANSI_COLOR_GREEN CHECK_HEAVY_MARK);
  }
  fprintf(stdout, " ");

  vfprintf(stdout, fmt, args);
  if (is_terminal(stdout)) {
    fprintf(stdout, ANSI_COLOR_RESET);
  }
  fprintf(stdout, "\n");
  va_end(args);
}

// Error print function: prints to stderr in red
void errorPrint(const char *fmt, ...) {
  va_list args;
  va_start(args, fmt);

  if (is_terminal(stderr)) {
    fprintf(stderr, ANSI_COLOR_RED ICON_RED_X);
  }

  vfprintf(stderr, fmt, args);
  if (is_terminal(stderr)) {
    fprintf(stderr, ANSI_COLOR_RESET);
  }
  fprintf(stderr, "\n");
  va_end(args);
}

int readArgs(int argc, char **argv) {
  if (argc == 2) {
    char *arg01 = argv[1];
    char c = arg01[0];
    if (c == '0') {
      normalPrint("Running all tests");
      return RUN_ALL_TESTS;
    }
    if (c == '1') {
      normalPrint("Only running test 1");
      return TEST01;
    } else if (c == '2') {
      normalPrint("Only running test 2");
      return TEST02;
    } else if (c == '3') {
      normalPrint("Only running test 3");
      return TEST03;
    } else if (c == '4') {
      normalPrint("Only running test 4");
      return TEST04;
    } else if (c == '5') {
      normalPrint("Only running test 5");
      return TEST05;
    } else {
      normalPrint("args: %s", arg01);
      errorPrint("Wrong use of arguments.");
      errorPrint(
          "Usage:\n\t atc   --- to run all tests\n\t atc 1 --- to run test 1, "
          "etc.");
    }
  }
  if (argc > 2) {
    errorPrint("Wrong use of arguments.");
    errorPrint(
        "Usage:\n\t atc --- to run all tests\n\t atc 1 --- to run test 1, "
        "etc.");
  }
  normalPrint("Running all tests.");
  return RUN_ALL_TESTS;
}