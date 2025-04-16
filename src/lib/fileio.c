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

#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <unistd.h>

#include "./fileio.h"

/*
  Creates new pair of metadata files and store files, and returns a structure with
  their file descriptors. the files will be empty. (THIS WILL OVERWRITE OLD STORES)
  check return status on error.
  The new backing will be opened by it's creation.
 */
status create_new_backing(char *filename, backing *store)
{
  status return_status = STATUS_OK;
  size_t namelen = strlen(filename);
  char storefilename[namelen+STOREEXTLEN+NULLLEN];
  meta_page *metadata = NULL;
  
  // build the filenames.
  memcpy(&storefilename, filename, namelen);
  memcpy(&storefilename[namelen], STOREEXT, STOREEXTLEN);
  
  // finish the file string.
  memset(&storefilename[namelen + STOREEXTLEN], '\0', NULLLEN);

  // open the files
  store->storefd = open(storefilename, O_CREAT | O_TRUNC | O_RDWR, S_IRWXU);

  // if open fails, close the file, just in case, release the memory and return an error
  if (store->storefd == -1) {
    close_backing(store);

    return_status = STATUS_BAD_FILE;
    goto cleanup;
  }

  // initialize metadata page 0.
  // blank the page, write the values into the struct, then write it to their
  // storage backing.
  metadata = calloc(1, PAGESIZE);
  metadata->size = 1; // there is only the metadata page.
  metadata->freelist_head.offset = -1; // no free pages to start.
  metadata->freelist_tail.offset = -1; // no free pages to start.
  
  ssize_t written = pwrite(store->storefd, metadata, PAGESIZE, index_to_offset(0));
  if (written != PAGESIZE){
    close_backing(store);

    return_status = STATUS_BAD_WRITE;
    goto cleanup;
  }

 cleanup:
  free(metadata);
  return return_status;
}

/*
  Open the store provided by the name, if it does not exist, NULL will
  be returned.
 */
status open_backing(char *filename, backing *store)
{
  size_t namelen = strlen(filename);
  char storefilename[namelen+STOREEXTLEN+NULLLEN];
  
  // build the filenames.
  memcpy(&storefilename, filename, namelen);
  
  memcpy(&storefilename[namelen], STOREEXT, STOREEXTLEN);

  // finish the file string.
  memset(&storefilename[namelen + STOREEXTLEN], '\0', NULLLEN);

  // open the files
  store->storefd = open(storefilename, O_RDWR);

  // if either open fails,  close the files, just in case, release the memory, and return NULL
  if (store->storefd == -1) {
    close_backing(store);

    return STATUS_BAD_OPEN;
  }
  
  return STATUS_OK;
}

/*
  close the files associated with the backing store. for errors, report it.
 */
status close_backing(backing* store)
{
  if (store != NULL){
    if(close(store->storefd) == 0){
      return STATUS_OK;
    }
    else{
      return STATUS_BAD_CLOSE;
    }
  }
  else{
    return STATUS_ERR;
  }
}

/*
  remove the named filestore from the disk.
  it is assumed that the files are not in use.
  
  return 0 if successful, -1 if the file
  requested does not exist.
 */
status remove_backing(char *filename)
{
  size_t namelen = strlen(filename);
  char storefilename[namelen+STOREEXTLEN+NULLLEN];
  
  // build the filenames.
  memcpy(&storefilename, filename, namelen);
  memcpy(&storefilename[namelen], STOREEXT, STOREEXTLEN);

  // finish the file string.
  memset(&storefilename[namelen + STOREEXTLEN], '\0', NULLLEN);

  // attempt to remove the files.
  if (remove(storefilename) != 0) {
    return STATUS_BAD_REMOVE;
  }
  
  return STATUS_OK;
}

// page manipulation.

/*
  given a page number, this function will return the offset
  in the backing to where the page starts.
 */
byte_offset index_to_offset(page_index index)
{
  // 0 indexed. works out nicely.
  return (index * PAGESIZE);
}

page_index offset_to_index(byte_offset offset)
{
  if (offset == 0) {
    return 0;
  }
  return (offset / PAGESIZE);
}

status alloc_page(void *src, page_index *index, backing *file)
{
  status return_status = STATUS_OK;
  meta_page *metadata = NULL;
  freepage *nextpage = NULL;  
  void *blank_page = NULL;
  
  if (file == NULL) {
    // no backing to read from!
    return_status = STATUS_NO_FILE;
    *index = ERR_SET;
    goto cleanup;
  }
  // read in the metadata.
  metadata = calloc(1, PAGESIZE);
  ssize_t read = pread(file->storefd, metadata, PAGESIZE, index_to_offset(0));
  if (read != PAGESIZE) {
    return_status = STATUS_BAD_READ;
    *index = ERR_SET;
    goto cleanup;
  }

  // if there are pages on the freelist, use one.
  if (metadata->freelist_head.offset != EMPTY_LIST) {
    byte_offset offset = metadata->freelist_head.offset;
    nextpage = calloc(1, (sizeof(freepage)));

    ssize_t read = pread(file->storefd, nextpage, sizeof(freepage), offset);
    if (read != sizeof(freepage)) {
      return_status = STATUS_BAD_READ;
      *index = ERR_SET;      
      goto cleanup;
    }

    if (metadata->freelist_head.offset == metadata->freelist_tail.offset) {
      metadata->freelist_head.offset = nextpage->offset;
      metadata->freelist_tail.offset = nextpage->offset;
    }
    else {
      metadata->freelist_head.offset = nextpage->offset;
    }

    ssize_t write = pwrite(file->storefd, src, PAGESIZE, offset);
    if (write != PAGESIZE) {
      return_status = STATUS_BAD_WRITE;
      *index = ERR_SET;
      goto cleanup;
    }

    ssize_t meta_write = pwrite(file->storefd, metadata, PAGESIZE, index_to_offset(0));
    if (meta_write != PAGESIZE) {
      return_status = STATUS_BAD_WRITE;
      *index = ERR_SET;
      goto cleanup;
    }

    // set the dest pointer, then go to cleanup
    *index = offset_to_index(offset);
    goto cleanup;
  }

  // if there are no free pages, make a new one.
  
  // current size is also the index of the next page to allocate.
  page_index newpage = metadata->size;
  blank_page = calloc(1, PAGESIZE);
  ssize_t blank_write = pwrite(file->storefd, blank_page, PAGESIZE, index_to_offset(newpage));
  if (blank_write != PAGESIZE) {
    return_status = STATUS_BAD_WRITE;
    *index = ERR_SET;
    goto cleanup;
  }

  ssize_t data_write = pwrite(file->storefd, src, PAGESIZE, index_to_offset(newpage));
  if (data_write != PAGESIZE) {
    return_status = STATUS_BAD_WRITE;
    *index = STATUS_ERR;
    goto cleanup;
  }

  metadata->size = metadata->size + 1; // size is now 1KB larger.

  ssize_t meta_write = pwrite(file->storefd, metadata, PAGESIZE, index_to_offset(0));
  if (meta_write != PAGESIZE) {
    return_status = STATUS_BAD_WRITE;
    *index = ERR_SET;
    goto cleanup;
  }

  *index = newpage;
  goto cleanup;
  
 cleanup:
  free(metadata);
  free(nextpage);
  free(blank_page);
  return return_status;
}

/*
  add the page referred to by `index` to the freelist, if the page exists.
  returns STATUS_OK if successful, check error types for errors.
 */
status free_page(backing *store, page_index index)
{
  status return_status = STATUS_OK;
  meta_page *metadata = NULL;
  freepage *new = NULL;
  freepage *old = NULL;
  
  byte_offset pageloc = index_to_offset(index);
  if (pageloc == 0) {
    // the metadata page is special. it cannot be freed.
    return_status = STATUS_META_PAGE;
    goto cleanup;
  }
  if (store == NULL) {
    // no backing to read from!
    return_status = STATUS_NO_FILE;
    goto cleanup;
  }

  // read in the metadata.
  metadata = calloc(1, PAGESIZE);
  ssize_t read = pread(store->storefd, metadata, PAGESIZE, index_to_offset(0));
  if (read != PAGESIZE) {
    return_status = STATUS_BAD_READ;
    goto cleanup;
  }

  // if there are no free pages, this becomes the first and last.
  if (metadata->freelist_tail.offset == -1 && metadata->freelist_head.offset == -1) {
    metadata->freelist_head.offset = pageloc;
    metadata->freelist_tail.offset = pageloc;

    freepage *new = calloc(1, PAGESIZE);
    new->offset = -1;

    ssize_t free_write = pwrite(store->storefd, new, PAGESIZE, pageloc);
    if (free_write != PAGESIZE) {
      return_status = STATUS_BAD_WRITE;
      goto cleanup;
    }
    ssize_t meta_write = pwrite(store->storefd, metadata, PAGESIZE, index_to_offset(0));
    if (meta_write != PAGESIZE) {
      return_status = STATUS_BAD_WRITE;
      goto cleanup;
    }
    goto cleanup;
  }

  // free list is not empty, append to the tail.

  old = calloc(1, PAGESIZE);
  old->offset = pageloc;

  ssize_t old_tail = pwrite(store->storefd, old, PAGESIZE, metadata->freelist_tail.offset);
  if (old_tail != PAGESIZE) {
    return_status = STATUS_BAD_WRITE;
    goto cleanup;
  }
  
  new = calloc(1, PAGESIZE);
  new->offset = -1;

  ssize_t free_write = pwrite(store->storefd, new, PAGESIZE, pageloc);
  if (free_write != PAGESIZE) {
    return_status = STATUS_BAD_WRITE;
    goto cleanup;
  }

  // set the new tail
  metadata->freelist_tail.offset = pageloc;
  ssize_t meta_write = pwrite(store->storefd, metadata, PAGESIZE, index_to_offset(0));
  if (meta_write != PAGESIZE) {
    return_status = STATUS_BAD_WRITE;
    goto cleanup;
  }

 cleanup:
  free(metadata);
  free(old);
  free(new);
  return return_status;
}
/*
  find the page, located at `index` in `file`, if it exists, and copy the
  data to `dest`.
  It is assumed that the `dest` pointer can hold at least
  PAGESIZE bytes.
 */
status get_page(void *dest, backing *store, page_index index)
{
  byte_offset pageloc = index_to_offset(index);
  if (pageloc == 0) {
    // the metadata page is special. no direct reads.
    return STATUS_META_PAGE;
  }
  if (store == NULL) {
    // no backing to read from!
    return STATUS_NO_FILE;
  }
  if (lseek(store->storefd, pageloc, SEEK_SET) == -1) {
    // page not found error?
    return STATUS_BAD_SEEK;
  }
  // attempt to read the page.
  ssize_t page_read = pread(store->storefd, dest, PAGESIZE, index_to_offset(index));
  if (page_read != PAGESIZE) {
    // read failed!
    return STATUS_BAD_READ;
  }
  
  return STATUS_OK;
}
/*
  given that the page exists, copy the full PAGESIZE from `src`
  to the page specified by `page_index`
 */
status put_page(void *src, backing *store, page_index index)
{
  byte_offset pageloc = index_to_offset(index);
  if (pageloc == 0) {
    // the metadata page is special. no direct reads.
    return STATUS_META_PAGE;
  }
  if (store == NULL) {
    return STATUS_NO_FILE;
  }
  if (lseek(store->storefd, pageloc, SEEK_SET) == -1) {
    return STATUS_BAD_SEEK;
  }
  // attempt to write the page.
  ssize_t page_write = pwrite(store->storefd, src, PAGESIZE, index_to_offset(index));
  if (page_write != PAGESIZE) {
    return STATUS_BAD_WRITE;
  }
  
  return STATUS_OK;
}
