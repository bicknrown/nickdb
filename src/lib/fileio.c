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

#define UUIDLEN 36
#define STOREEXTLEN 6
#define METAEXTLEN 5
#define NULLLEN 1

#define METAEXT ".meta"
#define STOREEXT ".store"

/*
  Creates new pair of metadata files and store files, and returns a structure with
  their file descriptors. the files will be empty. (THIS WILL OVERWRITE OLD STORES)
  Returns NULL on error.
  The new backing will be opened by it's creation.
 */
backing *create_new_backing_files(char *name)
{
  int namelen = strlen(name);
  char metauuidfilename[namelen+METAEXTLEN+NULLLEN];
  char storeuuidfilename[namelen+STOREEXTLEN+NULLLEN];
  
  // build the filenames.
  memcpy(&metauuidfilename, name, namelen);
  memcpy(&storeuuidfilename, name, namelen);
  
  memcpy(&metauuidfilename[namelen], METAEXT, METAEXTLEN);
  memcpy(&storeuuidfilename[namelen], STOREEXT, STOREEXTLEN);

  // finish the file string.
  memset(&metauuidfilename[namelen + METAEXTLEN], '\0', NULLLEN);
  memset(&storeuuidfilename[namelen + STOREEXTLEN], '\0', NULLLEN);

  // allocate the backing to return
  backing *files = calloc(1, sizeof(backing));

  // open the files
  files->metafd = open(metauuidfilename, O_CREAT | O_DIRECT | O_TRUNC | O_RDWR, S_IRWXU);
  files->storefd = open(storeuuidfilename, O_CREAT | O_DIRECT | O_TRUNC | O_RDWR, S_IRWXU);

  // if either open fails, release the memory and return NULL
  if (files->metafd == -1 || files->storefd == -1) {
    free(files);
    return NULL;
  }
  
  return files;
}

/*
  Open the store provided by the name, if it does not exist, NULL will
  be returned.
 */
backing *open_backing_store(char *name)
{
  int namelen = strlen(name);
  char metauuidfilename[namelen+METAEXTLEN+NULLLEN];
  char storeuuidfilename[namelen+STOREEXTLEN+NULLLEN];
  
  // build the filenames.
  memcpy(&metauuidfilename, name, namelen);
  memcpy(&storeuuidfilename, name, namelen);
  
  memcpy(&metauuidfilename[namelen], METAEXT, METAEXTLEN);
  memcpy(&storeuuidfilename[namelen], STOREEXT, STOREEXTLEN);

  // finish the file string.
  memset(&metauuidfilename[namelen + METAEXTLEN], '\0', NULLLEN);
  memset(&storeuuidfilename[namelen + STOREEXTLEN], '\0', NULLLEN);

  // allocate the backing to return
  backing *files = calloc(1, sizeof(backing));

  // open the files
  files->metafd = open(metauuidfilename, O_DIRECT | O_RDWR);
  files->storefd = open(storeuuidfilename, O_DIRECT | O_RDWR);

  // if either open fails, release the memory and return NULL
  if (files->metafd == -1 || files->storefd == -1) {
    free(files);
    return NULL;
  }
  
  return files;
}

/*
  close the files associated with the backing store, and free the structure.
  if the structure doesn't exist, why are you trying to close it?
 */
void close_backing_store(backing* files)
{
  if (files != NULL){
    close(files->storefd);
    close(files->metafd);
    free(files);
    files = NULL;
  }
  else {
    // ¯\_(ツ)_/¯
  }
}

