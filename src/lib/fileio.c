/*
 * Copyright 2025 Nick Brown <njbrown4@buffalo.edu>
 */

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <unistd.h>
#include <uuid/uuid.h>

#define UUIDLEN 36
#define EXTLEN 6
#define NULLLEN 1

#define STOREEXT ".store"

int create_unique_backing_store()
{
  int fd;
  char uuid[UUIDLEN+EXTLEN+NULLLEN];
  uuid_t uuidbin;

  uuid_generate(uuidbin);
  uuid_unparse(uuidbin, uuid);

  // i don't set the null,  i should do that
  memcpy(&uuid[UUIDLEN], STOREEXT, EXTLEN);

  fd = open(uuid, O_CREAT | O_DIRECT, S_IRWXU);
  
  return fd;
  
}

void close_backing_store(int backing) { close(backing); }

