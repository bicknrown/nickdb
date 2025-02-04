/*
 * Copyright 2025 Nick Brown <njbrown4@buffalo.edu>
 */

#include <stdlib.h>
#include <string.h>
#include <uuid/uuid.h>

#include "datastructures.h"

// functions
// 
// create a unique super structure, returns NULL if error.
store *create_store()
{
  store *newstore = NULL;

  newstore = calloc(1,sizeof(store));

  uuid_generate(newstore->id);
  
  return newstore;
}

// de-allocate the superstructure and everything in it.
int remove_store(store *store)
{
  if (store == NULL){
    return 0;
  }
  // TODO
  return 0;
}

// create a record with the given data. The data is copied to the new record.,
// returns NULL on error.
// note for removals, there are two allocations every time a record is created.
record *create_record(void *data, size_t size)
{
  record *newrecord = NULL;

  newrecord = calloc(1, sizeof(record));
  uuid_generate(newrecord->key);

  uuid_generate(newrecord->value->version);
  newrecord->value = calloc(1, sizeof(subrecord));
  newrecord->value->data = calloc(1, size);
  memcpy(newrecord->value->data, data, size);
  
  return newrecord;
}

int update_record(store *store, uuid_t key, void *data, size_t size)
{
  if (store == NULL){
    return 1;
  }
  // TODO
  return 0;
}

int append_record(store *store, record *record)
{
  if (store == NULL){
    return 1;
  }
  if (record == NULL){
    return 1;
  }

  // TODO
  
  return 0;
}

int insert_record(store *store, uuid_t key, record *record)
{
  if (store == NULL){
    return 1;
  }
  if (record == NULL){
    return 1;
  }
  // TODO
  return 0;
}

int remove_record(store *store, uuid_t key)
{
  if (store == NULL){
    return 1;
  }
  // TODO
  return 0;
}



