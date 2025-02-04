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

#include "./fileio.h"
#include "./datastructures.h"

#define UUIDLEN 36
#define STOREEXTLEN 6
#define METAEXTLEN 5
#define NULLLEN 1

#define METAEXT ".meta"
#define STOREEXT ".store"

int create_new_backing_store()
{
  char metauuidfilename[UUIDLEN+METAEXTLEN+NULLLEN];
  char storeuuidfilename[UUIDLEN+STOREEXTLEN+NULLLEN];
  uuid_t uuidbin;

  uuid_generate(uuidbin);
  uuid_unparse(uuidbin, metauuidfilename);
  uuid_unparse(uuidbin, storeuuidfilename);
  
  // build the filenames.
  memcpy(&metauuidfilename[UUIDLEN], METAEXT, METAEXTLEN);
  memcpy(&storeuuidfilename[UUIDLEN], STOREEXT, STOREEXTLEN);

  // finish the file string.
  memset(&metauuidfilename[UUIDLEN + METAEXTLEN], '\0', NULLLEN);
  memset(&storeuuidfilename[UUIDLEN + STOREEXTLEN], '\0', NULLLEN);
  
  return 0;
  
}

int open_backing_store(char *filename)
{
  int fd;
  
  //fd = open(uuid, O_CREAT | O_DIRECT, S_IRWXU);
    
  return fd;
}

void close_backing_store(backing* store)
{
  if (store != NULL){
    close(store->metafd);
    
  }
}

