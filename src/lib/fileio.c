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
status create_new_backing(char *name, backing **dest)
{
  status return_status = STATUS_OK;
  uint64_t namelen = strlen(name);
  char storefilename[namelen+STOREEXTLEN+NULLLEN];
  meta_page *metadata = NULL;
  
  // build the filenames.
  memcpy(&storefilename, name, namelen);
  memcpy(&storefilename[namelen], STOREEXT, STOREEXTLEN);
  
  // finish the file string.
  memset(&storefilename[namelen + STOREEXTLEN], '\0', NULLLEN);

  // open the files
  (*dest)->storefd = open(storefilename, O_CREAT | O_TRUNC | O_RDWR, S_IRWXU);

  // if open fails, close the file, just in case, release the memory and return an error
  if ((*dest)->storefd == -1) {
    close_backing((*dest));

    return_status = STATUS_BAD_FILE;
    *dest = NULL;
    goto cleanup;
  }

  // initialize metadata page 0.
  // blank the page, write the values into the struct, then write it to their
  // storage backing.
  metadata = calloc(1, PAGESIZE);
  metadata->size = 1; // there is only the metadata page.
  metadata->freelist_head.offset = -1; // no free pages to start.
  metadata->freelist_tail.offset = -1; // no free pages to start.
  
  ssize_t written = pwrite((*dest)->storefd, metadata, PAGESIZE, index_to_offset(0));
  if (written != PAGESIZE){
    close_backing((*dest));

    return_status = STATUS_BAD_WRITE;
    *dest = NULL;
    goto cleanup;
  }

  goto cleanup;
 cleanup:
  free(metadata);
  return return_status;
}

/*
  Open the store provided by the name, if it does not exist, NULL will
  be returned.
 */
status open_backing(char *name, backing **dest)
{
  uint64_t namelen = strlen(name);
  char storefilename[namelen+STOREEXTLEN+NULLLEN];
  
  // build the filenames.
  memcpy(&storefilename, name, namelen);
  
  memcpy(&storefilename[namelen], STOREEXT, STOREEXTLEN);

  // finish the file string.
  memset(&storefilename[namelen + STOREEXTLEN], '\0', NULLLEN);

  // open the files
  (*dest)->storefd = open(storefilename, O_RDWR);

  // if either open fails,  close the files, just in case, release the memory, and return NULL
  if ((*dest)->storefd == -1) {
    close_backing((*dest));

    *dest = NULL;
    return STATUS_BAD_OPEN;
  }
  
  return STATUS_OK;
}

/*
  close the files associated with the backing store. for errors, report it.
 */
status close_backing(backing* file)
{
  if (file != NULL){
    if(close(file->storefd == STATUS_OK)){
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
status remove_backing(char *name)
{
  uint64_t namelen = strlen(name);
  char storefilename[namelen+STOREEXTLEN+NULLLEN];
  
  // build the filenames.
  memcpy(&storefilename, name, namelen);
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
ssize_t index_to_offset(ssize_t page_index)
{
  // 0 indexed. works out nicely.
  return (page_index * PAGESIZE);
}

ssize_t offset_to_index(ssize_t offset)
{
  if (offset == 0) {
    return 0;
  }
  return (offset / PAGESIZE);
}

status alloc_page(void *src, ssize_t **dest, backing *file)
{
  status return_status = STATUS_OK;
  meta_page *metadata = NULL;
  freepage *nextpage = NULL;  
  void *blank_page = NULL;
  
  if (file == NULL) {
    // no backing to read from!
    return_status = STATUS_NO_FILE;
    *dest = NULL;
    goto cleanup;
  }
  // read in the metadata.
  metadata = calloc(1, PAGESIZE);
  int read = pread(file->storefd, metadata, PAGESIZE, index_to_offset(0));
  if (read != PAGESIZE) {
    return_status = STATUS_BAD_READ;
    *dest = NULL;    
    goto cleanup;
  }

  // if there are pages on the freelist, use one.
  if (metadata->freelist_head.offset != -1) {
    int offset = metadata->freelist_head.offset;
    nextpage = calloc(1, (sizeof(freepage)));

    int read = pread(file->storefd, nextpage, sizeof(freepage), offset);
    if (read != sizeof(freepage)) {
      return_status = STATUS_BAD_READ;
      *dest = NULL;      
      goto cleanup;
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
      return_status = STATUS_BAD_WRITE;
      *dest = NULL;
      goto cleanup;
    }

    int meta_write = pwrite(file->storefd, metadata, PAGESIZE, index_to_offset(0));
    if (meta_write != PAGESIZE) {
      return_status = STATUS_BAD_WRITE;
      *dest = NULL;
      goto cleanup;
    }

    // set the dest pointer, then go to cleanup
    **dest = offset_to_index(offset);
    goto cleanup;
  }

  // if there are no free pages, make a new one.
  
  // current size is also the index of the next page to allocate.
  int newpage = metadata->size;
  blank_page = calloc(1, PAGESIZE);
  int blank_write = pwrite(file->storefd, blank_page, PAGESIZE, index_to_offset(newpage));
  if (blank_write != PAGESIZE) {
    return_status = STATUS_BAD_WRITE;
    *dest = NULL;
    goto cleanup;
  }

  int data_write = pwrite(file->storefd, src, PAGESIZE, index_to_offset(newpage));
  if (data_write != PAGESIZE) {
    return_status = STATUS_BAD_WRITE;
    *dest = NULL;
    goto cleanup;
  }

  metadata->size = metadata->size + 1; // size is now 1KB larger.

  int meta_write = pwrite(file->storefd, metadata, PAGESIZE, index_to_offset(0));
  if (meta_write != PAGESIZE) {
    return_status = STATUS_BAD_WRITE;
    *dest = NULL;
    goto cleanup;
  }

  **dest = newpage;
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
status free_page(backing *file, ssize_t page_index)
{
  status return_status = STATUS_OK;
  meta_page *metadata = NULL;
  freepage *new = NULL;
  freepage *old = NULL;
  
  int pageloc = index_to_offset(page_index);
  if (pageloc == 0) {
    // the metadata page is special. it cannot be freed.
    return_status = STATUS_META_PAGE;
    goto cleanup;
  }
  if (file == NULL) {
    // no backing to read from!
    return_status = STATUS_NO_FILE;
    goto cleanup;
  }

  // read in the metadata.
  metadata = calloc(1, PAGESIZE);
  int read = pread(file->storefd, metadata, PAGESIZE, index_to_offset(0));
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

    int free_write = pwrite(file->storefd, new, PAGESIZE, pageloc);
    if (free_write != PAGESIZE) {
      return_status = STATUS_BAD_WRITE;
      goto cleanup;
    }
    int meta_write = pwrite(file->storefd, metadata, PAGESIZE, index_to_offset(0));
    if (meta_write != PAGESIZE) {
      return_status = STATUS_BAD_WRITE;
      goto cleanup;
    }
    goto cleanup;
  }

  // free list is not empty, append to the tail.

  old = calloc(1, PAGESIZE);
  old->offset = pageloc;

  int old_tail = pwrite(file->storefd, old, PAGESIZE, metadata->freelist_tail.offset);
  if (old_tail != PAGESIZE) {
    return_status = STATUS_BAD_WRITE;
    goto cleanup;
  }
  
  new = calloc(1, PAGESIZE);
  new->offset = -1;

  int free_write = pwrite(file->storefd, new, PAGESIZE, pageloc);
  if (free_write != PAGESIZE) {
    return_status = STATUS_BAD_WRITE;
    goto cleanup;
  }

  // set the new tail
  metadata->freelist_tail.offset = pageloc;
  int meta_write = pwrite(file->storefd, metadata, PAGESIZE, index_to_offset(0));
  if (meta_write != PAGESIZE) {
    return_status = STATUS_BAD_WRITE;
    goto cleanup;
  }

  goto cleanup;
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
status get_page(void **dest, backing *file, ssize_t page_index)
{
  ssize_t pageloc = index_to_offset(page_index);
  if (pageloc == 0) {
    // the metadata page is special. no direct reads.
    *dest = NULL;
    return STATUS_META_PAGE;
  }
  if (file == NULL) {
    // no backing to read from!
    *dest = NULL;
    return STATUS_NO_FILE;
  }
  if (lseek(file->storefd, pageloc, SEEK_SET) == -1) {
    // page not found error?
    *dest = NULL;
    return STATUS_BAD_SEEK;
  }
  // attempt to read the page.
  if (pread(file->storefd, *dest, PAGESIZE, index_to_offset(page_index)) != PAGESIZE) {
    // read failed!
    *dest = NULL;
    return STATUS_BAD_READ;
  }
  
  return STATUS_OK;
}
/*
  given that the page exists, copy the full PAGESIZE from `src`
  to the page specified by `page_index`
 */
status put_page(void *src, backing *file, ssize_t page_index)
{
  ssize_t pageloc = index_to_offset(page_index);
  if (pageloc == 0) {
    // the metadata page is special. no direct reads.
    return STATUS_META_PAGE;
  }
  if (file == NULL) {
    return STATUS_NO_FILE;
  }
  if (lseek(file->storefd, pageloc, SEEK_SET) == -1) {
    return STATUS_BAD_SEEK;
  }
  // attempt to read the page.
  ssize_t bytes = pwrite(file->storefd, src, PAGESIZE, index_to_offset(page_index));
  if (bytes != PAGESIZE) {
    return STATUS_BAD_WRITE;
  }
  
  return STATUS_OK;
}
