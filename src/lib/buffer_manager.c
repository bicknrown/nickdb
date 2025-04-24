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

#include "stdlib.h"
#include "string.h"

#include "./buffer_manager.h"

/*
  calculate the frame index from any meta_frame in the free list.
 */
frame_index get_frame_index(buffer_manager *man, meta_frame *meta)
{
  return (((void *)meta) - ((void *)man->metaframes)) / sizeof(meta_frame);
}

/*
  create an area in memory of size `frames` multiplied by PAGESIZE
  to work with data coming to and from disk.
  each manager is tied to its backing store, named by `storename`

  returns NULL in error.
  
 */
status buff_create(char *storename, buffer_manager **manager, int frames)
{
  backing *store = NULL;
  status backing_status = open_backing(storename, store);
  if (backing_status != STATUS_OK){
    return STATUS_NO_BACKING; 
  }

  // the meta structure itself.
  *manager = calloc(1, sizeof(buffer_manager));
  // initial metadata
  (*manager)->store = store;
  (*manager)->frames = frames;
  // the pointer for the actual buffer
  (*manager)->buffer = calloc(frames, sizeof(frame));
  // the metadata frame buffer
  (*manager)->metaframes = calloc(frames, sizeof(meta_frame));
  // the free list of frame starts with the the first frame.
  (*manager)->freelist = (*manager)->metaframes;
  // (all the frames are free!)
  for (int i=0; i < frames; i++) {
    // this should not walk of the end...
    if (i < frames - 1 ){
      (*manager)->metaframes[i].next_free_or_dirty = &(*manager)->metaframes[i + 1];
      (*manager)->metaframes[i].state = FS_UNPINNED;
    }
    else {
      (*manager)->metaframes[i].next_free_or_dirty = NULL;
      (*manager)->metaframes[i].state = FS_UNPINNED;
    }
  }
  // the write-back queue is empty to start.
  (*manager)->writeback = NULL;
  
  return STATUS_OK;
}

/*
  takes in a pointer to a buffer manager, write all dirty pages to disk,
  and then free each section of allocated memory from the manager,
  including the manager itself. and close the associated backing on disk.

  check status codes for errors.
 */
status buff_destroy(buffer_manager **manager)
{
  // if there is no manager, we are done here.
  if (manager == NULL){
    return STATUS_NO_MANAGER;
  }

  // for every dirty frame we have, we need to write it back to the disk store.
  for (meta_frame *meta = (*manager)->writeback; meta != NULL; meta = meta->next_free_or_dirty){
    int put = put_page((*manager)->buffer[get_frame_index((*manager), meta)], (*manager)->store, meta->index);
    if (put == 0) {
      meta->state = FS_UNPINNED;
    }
  }

  // i'm freeeeeeeee!
  free((*manager)->metaframes);
  free((*manager)->buffer);

  close_backing((*manager)->store);

  free(*manager);
  *manager = NULL;
  // free fallin'!
  
  return STATUS_OK;
}

/*
  take the page index from the argument, fetch it from the backing store, and
  store it in the buffer, setting it,s meta data correctly, and setting the
  return pointer `frame` to the pointer in the buffer array.
 */
status buff_pin(buffer_manager *manager, frame *pinned, page_index index)
{
  if (manager == NULL) {
    return STATUS_NO_MANAGER;
  }
  
  if (index == METADATA) {
    return STATUS_META_PAGE;
    // cannot work on the meta page.
  }

  if (manager->freelist == NULL) {
    // no more free pages!
    return STATUS_NO_FREE_FRAMES;
  }

  // claim the free page
  meta_frame *free_frame = manager->freelist;
  
  // move the manager onto the next one.
  *manager->freelist = *manager->freelist->next_free_or_dirty;

  frame_index frame = get_frame_index(manager, free_frame);
  
  status pin_status = get_page(manager->buffer[frame], manager->store, index);
  if (pin_status != STATUS_OK) {
    return pin_status;
  }

  // set the pointer for the region.
  pinned = &manager->buffer[frame];
  
  return STATUS_OK;
}

status buff_unpin(buffer_manager *manager, void *frame)
{
  //
  return STATUS_ERR;
}

status buff_mark_page(buffer_manager *manager, void *frame)
{
  //
  return STATUS_ERR;
}

status buff_flush_all(buffer_manager *manager)
{
  //
  return STATUS_ERR;
}
