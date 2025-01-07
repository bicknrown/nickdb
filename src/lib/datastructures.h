/*
 * Copyright 2025 Nick Brown <njbrown4@buffalo.edu>
 */

#include <uuid/uuid.h>

typedef struct subrecord subrecord;

typedef struct record record;

typedef struct store store;

store *create_store();

int remove_store(store *store);

record *create_record(void *data, size_t size);

int update_record(store *store, uuid_t key, void *data, size_t size);

int append_record(store *store, record *record);

int insert_record(store *store, uuid_t key, record *record);

int remove_record(store *store, uuid_t key);
