#ifndef PHONE_INCLUDED
#define PHONE_INCLUDED

#include "log.h"    // For ANSI colored logs
#include <stdio.h>  // For fopen
#include <stdlib.h> // malloc
#include <string.h> // memcpy
#include <unistd.h> // For getopt

// The amount of contacts allocated at once.
#ifndef BLOCK_ALLOCATION_CONTACT
#define BLOCK_ALLOCATION_CONTACT 5
#endif

typedef struct Contact {
  char name[21];
  char number[12];
} Contact;

typedef enum Mode { Undefined, Insert, Delete, Query, List } Mode;

typedef struct Database {
  // count of elements behind ref
  int count;
  // size allocated to database
  int size;
  Contact *ref;
} Database;

// Prototypes for subs implemented in phone.c
int insert(const char *filename, const char *backup);
int delete (const char *filename, const char *backup);
int query(const char *filename, const char *backup);
int list(const char *filename, const char *backup);

// Prototypes for subs implemented in phone_sub.c
int read_database(Database *db, const char *file_path);
int write_database(const Database *db, const char *file_path);
int insert_database(Database *db, const Contact new);
int delete_database(Database *db, const char *name);
int query_database(const Database *db, const char *name);
int list_database(const Database *db);
int new_database(Database *db, const int count);
int free_database(Database *db);

#endif /* end of include guard: PHONE_INCLUDED */
