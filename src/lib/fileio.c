/*
 * Copyright 2025 Nick Brown <njbrown4@buffalo.edu>
 */

#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <unistd.h>
#include <uuid/uuid.h>

#include "./fileio.h"

#define STOREEXTLEN 6
#define METAEXTLEN 5
#define NULLLEN 1

#define PAGESIZE 4096

#define METAEXT ".meta"
#define STOREEXT ".store"

/*
  close the files associated with the backing store, and free the structure.
  if the structure doesn't exist, why are you trying to close it?
 */
void close_backing(backing* files)
{
  if (files != NULL){
    close(files->storefd);
    close(files->metafd);
    free(files);
    files = NULL;
  }
}

/*
  Creates new pair of metadata files and store files, and returns a structure with
  their file descriptors. the files will be empty. (THIS WILL OVERWRITE OLD STORES)
  Returns NULL on error.
  The new backing will be opened by it's creation.
 */
backing *create_new_backing(char *name)
{
  int namelen = strlen(name);
  char metafilename[namelen+METAEXTLEN+NULLLEN];
  char storefilename[namelen+STOREEXTLEN+NULLLEN];
  
  // build the filenames.
  memcpy(&metafilename, name, namelen);
  memcpy(&storefilename, name, namelen);
  
  memcpy(&metafilename[namelen], METAEXT, METAEXTLEN);
  memcpy(&storefilename[namelen], STOREEXT, STOREEXTLEN);

  // finish the file string.
  memset(&metafilename[namelen + METAEXTLEN], '\0', NULLLEN);
  memset(&storefilename[namelen + STOREEXTLEN], '\0', NULLLEN);

  // allocate the backing to return
  backing *files = calloc(1, sizeof(backing));

  // open the files
  files->metafd = open(metafilename, O_CREAT | O_DIRECT | O_TRUNC | O_RDWR, S_IRWXU);
  files->storefd = open(storefilename, O_CREAT | O_DIRECT | O_TRUNC | O_RDWR, S_IRWXU);

  // if either open fails, close the files, just in case, release the memory and return NULL
  if (files->metafd == -1 || files->storefd == -1) {
    close_backing(files);
    return NULL;
  }
  
  return files;
}

/*
  Open the store provided by the name, if it does not exist, NULL will
  be returned.
 */
backing *open_backing(char *name)
{
  int namelen = strlen(name);
  char metafilename[namelen+METAEXTLEN+NULLLEN];
  char storefilename[namelen+STOREEXTLEN+NULLLEN];
  
  // build the filenames.
  memcpy(&metafilename, name, namelen);
  memcpy(&storefilename, name, namelen);
  
  memcpy(&metafilename[namelen], METAEXT, METAEXTLEN);
  memcpy(&storefilename[namelen], STOREEXT, STOREEXTLEN);

  // finish the file string.
  memset(&metafilename[namelen + METAEXTLEN], '\0', NULLLEN);
  memset(&storefilename[namelen + STOREEXTLEN], '\0', NULLLEN);

  // allocate the backing to return
  backing *files = calloc(1, sizeof(backing));

  // open the files
  files->metafd = open(metafilename, O_DIRECT | O_RDWR);
  files->storefd = open(storefilename, O_DIRECT | O_RDWR);

  // if either open fails,  close the files, just in case, release the memory, and return NULL
  if (files->metafd == -1 || files->storefd == -1) {
    close_backing(files);
    return NULL;
  }
  
  return files;
}

/*
  remove the named filestore from the disk.
  it is assumed that the files are not in use.
  
  return 0 if successful, -1 if the file
  requested does not exist.
 */
int remove_backing(char *name)
{
  int namelen = strlen(name);
  char metafilename[namelen+METAEXTLEN+NULLLEN];
  char storefilename[namelen+STOREEXTLEN+NULLLEN];
  
  // build the filenames.
  memcpy(&metafilename, name, namelen);
  memcpy(&storefilename, name, namelen);
  
  memcpy(&metafilename[namelen], METAEXT, METAEXTLEN);
  memcpy(&storefilename[namelen], STOREEXT, STOREEXTLEN);

  // finish the file string.
  memset(&metafilename[namelen + METAEXTLEN], '\0', NULLLEN);
  memset(&storefilename[namelen + STOREEXTLEN], '\0', NULLLEN);

  // attempt to remove the files.
  if (remove(metafilename) != 0) {
    return -1;
  }
  if (remove(storefilename)) {
    return -1;
  }
  
  return 0;
}

// page manipulation.

/*
  given a page number, this function will return the offset
  in the backing to where the page starts.
 */
int index_to_offset(int index)
{
  // 0 indexed. works out nicely.
  return (index * PAGESIZE);
}

/*
  find the page, located at `index` in `file`, and copy the
  data to `dest`.
  It is assumed that the `dest` pointer can hold at least
  PAGESIZE bytes.
 */
int get_page(void *dest, backing *file, int index)
{
  int pageloc = index_to_offset(index);
  if (file == NULL) {
    // no backing to read from!
    return -1;
  }
  if (lseek(file->storefd, pageloc, SEEK_SET) == -1) {
    // page not found error?
    return -1;
  }
  // attempt to read the page.
  if (read(file->storefd, dest, PAGESIZE) != PAGESIZE) {
    // read failed!
    return -1;
  }
  
  return 0;
}
/*
  
 */
int put_page(void *src, backing *file, int index)
{
  int pageloc = index_to_offset(index);
  if (file == NULL) {
    fprintf(stderr,"no backing to read from!\n");
    return -1;
  }
  if (lseek(file->storefd, pageloc, SEEK_SET) == -1) {
    fprintf(stderr,"page %i not found\n", index);
    return -1;
  }
  // attempt to read the page.
  int bytes = write(file->storefd, src, PAGESIZE);
  if (bytes != PAGESIZE) {
    fprintf(stderr,"write() failed!, %i bytes written.\n errno: %i\npageoffset: %i\n", bytes, errno, pageloc);
    perror("error: ");
    return -1;
  }
  
  return 0;
}
