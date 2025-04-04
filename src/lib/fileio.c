/*
   Copyright 2025 Nick Brown <njbrown4@buffalo.edu>

   This file is part of nickdb.

   nickdb is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   nickdb is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <https://www.gnu.org/licenses/>.

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
#include "./constants.h"

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
  backing *file = calloc(1, sizeof(backing));

  // open the files
  file->metafd = open(metafilename, O_CREAT | O_TRUNC | O_RDWR, S_IRWXU);
  file->storefd = open(storefilename, O_CREAT | O_TRUNC | O_RDWR, S_IRWXU);

  // if either open fails, close the files, just in case, release the memory and return NULL
  if (file->metafd == -1 || file->storefd == -1) {
    close_backing(file);
    return NULL;
  }

  // initialize metadata page 0.
  // blank the page, write the values into the struct, then write it to their
  // storage backing.
  meta_page *metadata = calloc(1, PAGESIZE);
  metadata->size = 1; // there is only the metadata page.
  metadata->freelist_head.offset = -1; // no free pages to start.
  metadata->freelist_tail.offset = -1; // no free pages to start.
  
  int written = pwrite(file->storefd, metadata, PAGESIZE, index_to_offset(0));
  free(metadata);
  metadata = NULL;
  if (written != PAGESIZE){
    close_backing(file);
    return NULL;
  }
  
  return file;
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
  files->metafd = open(metafilename, O_RDWR);
  files->storefd = open(storefilename, O_RDWR);

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
  if (remove(storefilename) != 0) {
    return -1;
  }
  
  return 0;
}

// page manipulation.

/*
  given a page number, this function will return the offset
  in the backing to where the page starts.
 */
int index_to_offset(int page_index)
{
  // 0 indexed. works out nicely.
  return (page_index * PAGESIZE);
}

int offset_to_index(int offset)
{
  if (offset == 0) {
    return 0;
  }
  return (offset / PAGESIZE);
}

int alloc_page(void *src, backing *file)
{
  if (file == NULL) {
    // no backing to read from!
    return -1;
  }
  // read in the metadata.
  meta_page *metadata = calloc(1, PAGESIZE);
  int read = pread(file->storefd, metadata, PAGESIZE, index_to_offset(0));
  if (read != PAGESIZE) {
    return -1;
  }
  
  // if there are pages on the freelist, use one.
  if (metadata->freelist_head.offset != -1) {
    int offset = metadata->freelist_head.offset;
    int returnindex = offset_to_index(offset);
    freepage *nextpage = calloc(1, (sizeof(freepage)));

    int read = pread(file->storefd, nextpage, sizeof(freepage), offset);
    if (read != sizeof(freepage)) {
      free(metadata);
      free(nextpage);
      return -1;
    }

    if (metadata->freelist_head.offset == metadata->freelist_tail.offset) {
      metadata->freelist_head.offset = nextpage->offset;
      metadata->freelist_tail.offset = nextpage->offset;
    }
    else {
      metadata->freelist_head.offset = nextpage->offset;
    }

    int write = pwrite(file->storefd, src, PAGESIZE, offset);
    if (write != PAGESIZE) {
      free(metadata);
      free(nextpage);
      return -1;
    }

    int meta_write = pwrite(file->storefd, metadata, PAGESIZE, index_to_offset(0));
    if (meta_write != PAGESIZE) {
      free(metadata);
      free(nextpage);
      return -1;
    }
    free(metadata);
    free(nextpage);
    return returnindex;
  }

  // if there are no free pages, make a new one.
  
  // current size is also the index of the next page to allocate.
  int newpage = metadata->size;
  void *blank_page = calloc(1, PAGESIZE);
  int blank_write = pwrite(file->storefd, blank_page, PAGESIZE, index_to_offset(newpage));
  if (blank_write != PAGESIZE) {
    free(metadata);
    free(blank_page);
    return -1;
  }

  int data_write = pwrite(file->storefd, src, PAGESIZE, index_to_offset(newpage));
  if (data_write != PAGESIZE) {
    free(metadata);
    free(blank_page);
    return -1;
  }

  metadata->size = metadata->size + 1; // size is now 1KB larger.

  int meta_write = pwrite(file->storefd, metadata, PAGESIZE, index_to_offset(0));
  if (meta_write != PAGESIZE) {
    free(metadata);
    free(blank_page);
    return -1;
  }
  
  free(metadata);
  free(blank_page);
  return newpage;
}

/*
  add the page referred to by `index` to the freelist, if the page exists.
  returns 0 if successful, -1 in error.
 */
int free_page(backing *file, int index)
{
  int pageloc = index_to_offset(index);
  if (pageloc == 0) {
    // the metadata page is special. it cannot be freed.
    return -1;
  }
  if (file == NULL) {
    // no backing to read from!
    return -1;
  }

  // read in the metadata.
  meta_page *metadata = calloc(1, PAGESIZE);
  int read = pread(file->storefd, metadata, PAGESIZE, index_to_offset(0));
  if (read != PAGESIZE) {
    return -1;
  }

  // if there are no free pages, this becomes the first and last.
  if (metadata->freelist_tail.offset == -1 && metadata->freelist_head.offset == -1) {
    metadata->freelist_head.offset = pageloc;
    metadata->freelist_tail.offset = pageloc;

    freepage *new = calloc(1, PAGESIZE);
    new->offset = -1;

    int free_write = pwrite(file->storefd, new, PAGESIZE, pageloc);
    if (free_write != PAGESIZE) {
      free(metadata);
      free(new);
      return -1;
    }
    int meta_write = pwrite(file->storefd, metadata, PAGESIZE, index_to_offset(0));
    if (meta_write != PAGESIZE) {
      free(metadata);
      free(new);
      return -1;
    }
    free(new);
  }

  // free list is not empty, append to the tail.

  freepage *old = calloc(1, PAGESIZE);
  old->offset = pageloc;

  int old_tail = pwrite(file->storefd, old, PAGESIZE, metadata->freelist_tail.offset);
  if (old_tail != PAGESIZE) {
    free(metadata);
    free(old);
    return -1;
  }
  
  freepage *new = calloc(1, PAGESIZE);
  new->offset = -1;

  int free_write = pwrite(file->storefd, new, PAGESIZE, pageloc);
  if (free_write != PAGESIZE) {
    free(metadata);
    free(old);
    free(new);
    return -1;
  }

  // set the new tail
  metadata->freelist_tail.offset = pageloc;
  int meta_write = pwrite(file->storefd, metadata, PAGESIZE, index_to_offset(0));
  if (meta_write != PAGESIZE) {
    free(metadata);
    free(old);
    free(new);
    return -1;
  }
  
  free(metadata);
  free(old);
  free(new);
  return 0;
}
/*
  find the page, located at `index` in `file`, if it exists, and copy the
  data to `dest`.
  It is assumed that the `dest` pointer can hold at least
  PAGESIZE bytes.
 */
int get_page(void *dest, backing *file, int page_index)
{
  int pageloc = index_to_offset(page_index);
  if (pageloc == 0) {
    // the metadata page is special. no direct reads.
    return -1;
  }
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
  given that the page exists, copy the full PAGESIZE from `src`
  to the page specified by `page_index`
 */
int put_page(void *src, backing *file, int page_index)
{
  int pageloc = index_to_offset(page_index);
  if (pageloc == 0) {
    // the metadata page is special. no direct reads.
    return -1;
  }
  if (file == NULL) {
    fprintf(stderr,"no backing to read from!\n");
    return -1;
  }
  if (lseek(file->storefd, pageloc, SEEK_SET) == -1) {
    fprintf(stderr,"page %i not found\n", page_index);
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
