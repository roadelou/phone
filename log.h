#ifndef LOG_PHONE_INCLUDED
#define LOG_PHONE_INCLUDED

#include <stdarg.h> // For variadic functions
#include <stdio.h>  // For printf
#include <stdlib.h> // for exit

// All the following function use printf format.

// prints error message to user
int error_log(const char *msg, ...);

// prints user message
int info_log(const char *msg, ...);

// print informations about internal actions
int action_log(const char *msg, ...);

#endif /* end of include guard: LOG_PHONE_INCLUDED */
