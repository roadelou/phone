#include "phone.h"

// The maximum amount of contatcs or requests that can be processed at once.
static char name_list[BLOCK_ALLOCATION_CONTACT][21];
static char number_list[BLOCK_ALLOCATION_CONTACT][11];
static int cursor;

int main(int argc, const char *argv[]) {
  // cli arguments parsing with getopt
  int opt;
  Mode mode = Undefined;
  cursor = 0;
  char *filename = NULL;
  while ((opt = getopt(argc, (char *const *)argv, ":i:d:q:lf:")) != -1) {
    switch (opt) {
    case 'i':
      if ((mode == Undefined) || (mode == Insert)) {
        mode = Insert;
        // action_log("Insert mode.");
        if (cursor == BLOCK_ALLOCATION_CONTACT) {
          error_log("Too many arguments to process at once, the maximum is %d.",
                    BLOCK_ALLOCATION_CONTACT);
          return -1;
        } else {
          strncpy(name_list[cursor], argv[optind - 1], 21);
          strncpy(number_list[cursor], argv[optind], 11);
          optind++;
          cursor++;
        }
      } else {
        error_log("Several contradicting actions specified.");
        return -1;
      }
      break;
    case 'd':
      if ((mode == Undefined) || (mode == Delete)) {
        mode = Delete;
        // action_log("Delete mode.");
        if (cursor == BLOCK_ALLOCATION_CONTACT) {
          error_log("Too many arguments to process at once, the maximum is %d.",
                    BLOCK_ALLOCATION_CONTACT);
          return -1;
        } else {
          memcpy(name_list[cursor], optarg, 21);
          cursor++;
        }
      } else {
        error_log("Several contradicting actions specified.");
        return -1;
      }
      break;
    case 'q':
      if ((mode == Undefined) || (mode == Query)) {
        mode = Query;
        // action_log("Query mode.");
        if (cursor == BLOCK_ALLOCATION_CONTACT) {
          error_log("Too many arguments to process at once, the maximum is %d.",
                    BLOCK_ALLOCATION_CONTACT);
          return -1;
        } else {
          memcpy(name_list[cursor], optarg, 21);
          cursor++;
        }
      } else {
        error_log("Several contradicting actions specified.");
        return -1;
      }
      break;
    case 'l':
      if ((mode == Undefined) || (mode == List)) {
        mode = List;
        // action_log("List mode.");
      } else {
        error_log("Several contradicting actions specified.");
        return -1;
      }
      break;
    case 'f':
      filename = optarg;
      action_log("Using database file %s.", filename);
      break;
    case ':':
      error_log("Option -%c is missing its operand.", optopt);
      return -1;
    case '?':
      error_log("Unknown option -%c", optopt);
      break;
    }
  }

  if (mode == Undefined) {
    info_log("Nothing to do...");
    return 1;
  }
  // else...

  char *home = getenv("HOME");
  int backup_len = strlen(home) + strlen(".phone_backup") + 2;
  char *backup_name = malloc(backup_len);
  snprintf(backup_name, backup_len, "%s/%s", home, ".phone_backup");
  action_log("Backup to %s", backup_name);
  if (filename == NULL) {
    int filename_len = strlen(home) + strlen(".phone") + 2;
    filename = malloc(filename_len);
    snprintf(filename, filename_len, "%s/%s", home, ".phone");
    action_log("No database name given, assuming %s", filename);
  }

  switch (mode) {
  case Insert:
    insert(filename, backup_name);
    break;
  case Delete:
    delete (filename, backup_name);
    break;
  case Query:
    query(filename, backup_name);
    break;
  case List:
    list(filename, backup_name);
    break;
  default:
    error_log("Invalid action detected, mode was somehow set to an "
              "inconsistent value.");
    break;
  }

  // free(filename);
  // free(backup_name);
  return 0;
}

int insert(const char *filename, const char *backup) {
  Database db;
  int status;
  new_database(&db, BLOCK_ALLOCATION_CONTACT);
  FILE *test_exists = fopen(filename, "rb");
  if (test_exists != NULL) {
    // File refers to an already existing database
    action_log("Reading database %s.", filename);
    status = read_database(&db, filename);
    if (status == -1) {
      // Error propagation
      return -1;
    } else if (status > 0) {
      // Allocating more size to fit database.
      new_database(&db, BLOCK_ALLOCATION_CONTACT + status);
      // Trying to read again.
      status = read_database(&db, filename);
      if (status != 0) {
        // error_log("Could not read database again after memory allocation.");
        fclose(test_exists);
        return -1;
      }
      // else database has been read correctly
    }
    // else database has been read correctly on first try
    // Creating the backup.
    status = write_database(&db, backup);
    if (status == -1) {
      // error_log("Could not create backup file %s.", backup);
      fclose(test_exists);
      return -1;
    } else {
      action_log("Backup of file %s created at %s.", filename, backup);
    }
    // safe to proceed, closing file.
    fclose(test_exists);
  }

  Contact elt;
  for (size_t i = 0; i < cursor; i++) {
    memcpy(elt.name, name_list[i], 21);
    memcpy(elt.number, number_list[i], 11);
    status = insert_database(&db, elt);
    if (status == -1) {
      return -1;
    }
  }

  status = write_database(&db, filename);
  if (status == -1) {
    return -1;
  } else {
    free_database(&db);
    return 0;
  }
}

int delete (const char *filename, const char *backup) {
  Database db;
  int status;
  new_database(&db, BLOCK_ALLOCATION_CONTACT);
  FILE *test_exists = fopen(filename, "rb");
  if (test_exists != NULL) {
    // File refers to an already existing database
    action_log("Reading database %s.", filename);
    status = read_database(&db, filename);
    if (status == -1) {
      // Error propagation
      fclose(test_exists);
      return -1;
    } else if (status > 0) {
      // Allocating more size to fit database.
      new_database(&db, BLOCK_ALLOCATION_CONTACT + status);
      // Trying to read again.
      status = read_database(&db, filename);
      if (status != 0) {
        // error_log("Could not read database again after memory allocation.");
        fclose(test_exists);
        return -1;
      }
      // else database has been read correctly
    }
    // else database has been read correctly on first try
    // Creating the backup.
    status = write_database(&db, backup);
    if (status == -1) {
      // error_log("Could not create backup file %s.", backup);
      fclose(test_exists);
      return -1;
    } else {
      action_log("Backup of file %s created at %s.", filename, backup);
    }
    // safe to proceed, closing file.
    fclose(test_exists);
  }

  for (size_t i = 0; i < cursor; i++) {
    status = delete_database(&db, name_list[i]);
    if (status == -1) {
      return -1;
    }
  }

  status = write_database(&db, filename);
  if (status == -1) {
    return -1;
  } else {
    free_database(&db);
    return 0;
  }
}

int query(const char *filename, const char *backup) {
  Database db;
  int status;
  new_database(&db, BLOCK_ALLOCATION_CONTACT);
  FILE *test_exists = fopen(filename, "rb");
  if (test_exists != NULL) {
    // File refers to an already existing database
    action_log("Reading database %s.", filename);
    status = read_database(&db, filename);
    if (status == -1) {
      // Error propagation
      fclose(test_exists);
      return -1;
    } else if (status > 0) {
      // Allocating more size to fit database.
      new_database(&db, BLOCK_ALLOCATION_CONTACT + status);
      // Trying to read again.
      status = read_database(&db, filename);
      if (status != 0) {
        // error_log("Could not read database again after memory allocation.");
        fclose(test_exists);
        return -1;
      }
      // else database has been read correctly
    }
    // else database has been read correctly on first try

    // Creating the backup.
    status = write_database(&db, backup);
    if (status == -1) {
      // error_log("Could not create backup file %s.", backup);
      fclose(test_exists);
      return -1;
    } else {
      action_log("Backup of file %s created at %s.", filename, backup);
    }
    // safe to proceed, closing file.
    fclose(test_exists);
  } else {
    error_log("No database file found at %s.", filename);
    return -1;
  }

  for (size_t i = 0; i < cursor; i++) {
    status = query_database(&db, name_list[i]);
    if (status == -1) {
      return -1;
    }
  }

  // Nothing to write.
  free_database(&db);
  return 0;
}

int list(const char *filename, const char *backup) {
  Database db;
  int status;
  new_database(&db, BLOCK_ALLOCATION_CONTACT);
  FILE *test_exists = fopen(filename, "rb");
  if (test_exists != NULL) {
    // File refers to an already existing database
    action_log("Reading database %s.", filename);
    status = read_database(&db, filename);
    if (status == -1) {
      // Error propagation
      fclose(test_exists);
      return -1;
    } else if (status > 0) {
      // Allocating more size to fit database.
      new_database(&db, BLOCK_ALLOCATION_CONTACT + status);
      // Trying to read again.
      status = read_database(&db, filename);
      if (status != 0) {
        // error_log("Could not read database again after memory allocation.");
        fclose(test_exists);
        return -1;
      }
      // else database has been read correctly
    }
    // else database has been read correctly on first try
    // Creating the backup.
    status = write_database(&db, backup);
    if (status == -1) {
      // error_log("Could not create backup file %s.", backup);
      fclose(test_exists);
      return -1;
    } else {
      action_log("Backup of file %s created at %s.", filename, backup);
    }
    // safe to proceed,closing file.
    fclose(test_exists);
  } else {
    error_log("No database file found at %s.", filename);
    return -1;
  }

  status = list_database(&db);
  if (status == -1) {
    return -1;
  }

  // Nothing to write.
  free_database(&db);
  return 0;
}
