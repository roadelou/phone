#include "phone.h"

int read_database(Database *db, const char *file_path) {
  FILE *data_file = fopen(file_path, "rb");
  // Getting number of contacts in file.
  int contact_count;
  size_t status = fread(&contact_count, 1, sizeof(int), data_file);
  if (status != sizeof(int)) {
    error_log("Cannot read from file %s.", file_path);
    fclose(data_file);
    return -1; // < 0
  }
  // else...
  if (contact_count > db->size) {
    action_log("Buffer size too small, request reallocation.");
    // Disregarding cast issues, highly unlikely for this file size
    fclose(data_file);
    return contact_count; // > 0
  }
  // else...
  // Setting contact to zero
  memset(db->ref, 0, db->size);
  // Reading contacts from file
  status = fread(db->ref, sizeof(Contact), contact_count, data_file);
  if (status != contact_count) {
    error_log("Database file inconsistent, announced %d elements but could "
              "only read %d.",
              contact_count, status / sizeof(Contact));
    fclose(data_file);
    return -1;
  } else {
    action_log("Successfully read database from file %s.", file_path);
    fclose(data_file);
    db->count = contact_count;
    return 0;
  }
}

int write_database(const Database *db, const char *file_path) {
  FILE *data_file = fopen(file_path, "wb");
  // Writing count of phone numbers in the file.
  size_t status = fwrite(&(db->count), 1, sizeof(int), data_file);
  if (status != sizeof(int)) {
    error_log("Cannot write to file %s.", file_path);
    fclose(data_file);
    return -1; // < 0
  }
  // else...
  // Writing the bulk of the database
  status = fwrite(db->ref, sizeof(Contact), db->count, data_file);
  if (status != db->count) {
    error_log("Could not write database to file %s, the file might have been "
              "corrupted.",
              file_path);
    fclose(data_file);
    return -1; // < 0
  } else {
    action_log("Successfully wrote database to file %s.", file_path);
    fclose(data_file);
    return 0;
  }
}

int insert_database(Database *db, const Contact contact) {
  // If database is full
  if (db->count == db->size) {
    void *reallocation = realloc(
        db->ref, (db->size + BLOCK_ALLOCATION_CONTACT) * sizeof(Contact));
    if (reallocation == NULL) {
      error_log("Could not allocate memory for contact %s.", contact.name);
      return -1;
    } else {
      action_log("Database memory reallocation.");
      db->ref = reallocation;
      db->size += 5;
    }
  }
  // Copying contact to database
  memcpy(db->ref + db->count, &contact, sizeof(Contact));
  db->count++;
  info_log("Successfully inserted %s into database.", contact.name);
  return 1;
}

int delete_database(Database *db, const char *name) {
  // Searching elements through database
  Contact elt;
  int delete_count = 0;
  for (size_t i = 0; i < db->count; i++) {
    elt = db->ref[i];
    if (strncmp(elt.name, name, 21) == 0) {
      // Found a contact to delete
      memmove(db->ref + i, db->ref + i + 1, (db->count - i) * sizeof(Contact));
      db->count--;
      delete_count++;
      action_log("Deleted contact %s with number %s.", elt.name, elt.number);
    }
  }
  info_log("Successfully deleted %d contacts.", delete_count);
  return 0;
}

int query_database(const Database *db, const char *name) {
  Contact elt;
  int match_count = 0;
  for (size_t i = 0; i < db->count; i++) {
    elt = db->ref[i];
    if (strncmp(elt.name, name, 21) == 0) {
      // Found a contact to delete
      info_log("Found contact %s with number %s.", elt.name, elt.number);
      match_count++;
    }
  }
  info_log("Successfully matched %d contacts.", match_count);
  return 0;
}

int list_database(const Database *db) {
  Contact elt;
  info_log("Database contains %d elements.", db->count);
  action_log("Database allocated size is enough to hold %d contacts.",
             db->size);
  for (size_t i = 0; i < db->count; i++) {
    elt = db->ref[i];
    info_log("Contact %s with number %s.", elt.name, elt.number);
  }
  return 0;
}

int new_database(Database *db, const int count) {
  void *allocation = malloc(count * sizeof(Contact));
  if (allocation == NULL) {
    error_log("Could not allocate memory for new database.");
    return -1;
  } else {
    action_log("Allocated enough memory to fit %d contacts to database.",
               count);
    db->count = 0;
    db->size = count;
    db->ref = allocation;
  }
  return 0;
}

int free_database(Database *db) {
  free(db->ref);
  return 0;
}
