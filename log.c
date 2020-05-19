#include "log.h"

int error_log(const char *msg, ...) {
  va_list args;
  va_start(args, msg);
  // prints in red
  printf("%s", "\033[91m---X ");
  vprintf(msg, args);
  // ANSI escape
  printf("%s", "\033[m\n");
  va_end(args);
  return 0;
}

int info_log(const char *msg, ...) {
  va_list args;
  va_start(args, msg);
#ifndef PHONE_RELEASE
  // prints in green
  printf("%s", "\033[92m>> ");
  vprintf(msg, args);
  // ANSI escape
  printf("%s", "\033[m\n");
#else
  putchar('>');
  putchar('>');
  putchar(' ');
  vprintf(msg, args);
  putchar('\n');
#endif
  va_end(args);
  return 0;
}

int action_log(const char *msg, ...) {
#ifndef PHONE_RELEASE
  va_list args;
  va_start(args, msg);
  // // prints in yellow
  // puts("\033[93m>> ");
  // printf("%s", ">> ");
  // Just prints in white.
  vprintf(msg, args);
  putchar('\n');

  // // ANSI escape
  // puts("\033[m\n");
  va_end(args);
#endif
  return 0;
}
