/*
 * Copyright 2025 Nick Brown <njbrown4@buffalo.edu>
 */

#include <uuid/uuid.h>


typedef struct backing {
  int metafd;
  int storefd;
}backing;

/*
// a subrecord consists of a single "page" of data at a particular version.
typedef struct subrecord {
  void *data;
  uuid_t version;
}subrecord;

// a record holds three references to previous and future versions,
// as well as the current version. this linked list method has drawbacks,
// but for the majority of cases should be *okay*.
typedef struct record {
  uuid_t key;
  
  subrecord *forward;
  subrecord *back;
  
  subrecord *value;

  //record *next;
  //record *prev;
}record;

// a store is a collection of records, effectively the "database" super
// structure.
// currently a linked list structure, but a tree would make this better.
typedef struct store {
  uuid_t id;
  record *head;
  record *tail;
}store;

store *create_store();

int remove_store(store *store);

record *create_record(void *data, size_t size);

int update_record(store *store, uuid_t key, void *data, size_t size);

int append_record(store *store, record *record);

int insert_record(store *store, uuid_t key, record *record);

int remove_record(store *store, uuid_t key);
*/
