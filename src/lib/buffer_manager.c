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

#include <stdlib.h>
#include <string.h>

#include "./buffer_manager.h"
#include "constants.h"
#include "fileio.h"

TODO("buffer_manager- factor out metadata page reads and writes to the structure on manager creation.")

/*
  calculate the frame pointer from any meta_frame in the free list.
 */
frame_index get_frame_index_from_meta(buffer_manager *man, meta_frame *meta)
{
  return (((void *)meta) - ((void *)man->metaframes)) / sizeof(meta_frame);
}

/*
  calculate the frame pointer from any frame in the buffer.
 */
frame_index get_frame_index_from_frame(buffer_manager *man, frame *frame_pointer)
{
  return (((void *)frame_pointer) - ((void *)man->buffer)) / sizeof(frame);
}

/*
  create an area in memory of size `frames` multiplied by PAGESIZE
  to work with data coming to and from disk.
  each manager is tied to its backing store, named by `storename`

  returns NULL in error.

 */
TODO("`buff_create()`- move linked list operations out to a datastructure")
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

  // initialize the lookup hashtable.
  (*manager)->lookup_table = g_hash_table_new_full(NULL, NULL, free, free);
  
  return STATUS_OK;
}

/*
  takes in a pointer to a buffer manager, write all dirty pages to disk,
  and then free each section of allocated memory from the manager,
  including the manager itself. and close the associated backing on disk.

  check status codes for errors.
 */
TODO("`buff_destroy()`- move linked list operations out to a datastructure")
status buff_destroy(buffer_manager **manager)
{
  // if there is no manager, we are done here.
  if (manager == NULL){
    return STATUS_NO_MANAGER;
  }

  // for every dirty frame we have, we need to write it back to the disk store.
  for (meta_frame *meta = (*manager)->writeback; meta != NULL; meta = meta->next_free_or_dirty){
    status put = put_page((*manager)->buffer[get_frame_index_from_meta((*manager), meta)], (*manager)->store, meta->index);
    if (put == STATUS_OK) {
      meta->state = FS_UNPINNED;
    }
  }

  // i'm freeeeeeeee!
  free((*manager)->metaframes);
  free((*manager)->buffer);

  g_hash_table_destroy((*manager)->lookup_table);
  
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
TODO("`buff_pin()`- move linked list operations out to a datastructure")
status buff_pin(buffer_manager *manager, frame **frame, page_index index)
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
  
  frame_index frameidx = get_frame_index_from_meta(manager, free_frame);

  // init for adding the binding to the hashtable.
  page_index pageidxptr = ERR_SET;
  frame_index frameidxptr = ERR_SET;
  pageidxptr = index;
  frameidxptr = frameidx;
  
  // check if it exists in the table first.
  gpointer *existing_frame = g_hash_table_lookup(manager->lookup_table, &pageidxptr);
  
  if (existing_frame == NULL) {
      gboolean insert = g_hash_table_insert(manager->lookup_table, &pageidxptr, &frameidxptr);
      if (insert != true) {
	return STATUS_ERR;
      }
  }
  // if the page is already pinned, just return the frame pointer.
  else {
    *frame = &manager->buffer[frameidx];
    return STATUS_OK;
  }

  // actually fetch the data from the disk.
  status pin_status = get_page(manager->buffer[frameidx], manager->store, index);
  if (pin_status != STATUS_OK) {
    return pin_status;
  }
  // move the manager onto the next one.
  *manager->freelist = *manager->freelist->next_free_or_dirty;

  // actually mark the page as pinned.
  manager->metaframes[frameidx].state = FS_PINNED;

  // set the pageid into the metadata frame
  manager->metaframes[frameidx].index = index;
  
  // set the pointer for the region.
  *frame = &manager->buffer[frameidx];
  
  return STATUS_OK;
}

/*
  using the `frame`, we check if the page is dirty or clean. if it is clean,
  then we mark the frame's metadata as FS_UNPINNED. If it is dirty, we make the
  frame's meta data with FS_UNPINNED_DIRTY, and add it to writeback list.
 */
TODO("`buff_unpin()`- move linked list operations out to a datastructure")
status buff_unpin(buffer_manager *manager, frame *frame)
{
  if (manager == NULL) {
    return STATUS_NO_MANAGER;
  }
  frame_index frameidx = get_frame_index_from_frame(manager, frame);

  if (manager->metaframes[frameidx].state == FS_PINNED_DIRTY){
    manager->metaframes[frameidx].state = FS_UNPINNED_DIRTY;

    // hashtable removal
    page_index *pageidxptr = calloc(1, sizeof(page_index));
    *pageidxptr = manager->metaframes[frameidx].index;
    gboolean remove_key = g_hash_table_remove(manager->lookup_table, pageidxptr);
    if (remove_key != true) {
      free(pageidxptr);
    }
    if (manager->writeback != NULL){
      meta_frame *oldhead = manager->writeback;
      manager->writeback = &manager->metaframes[frameidx];
      manager->metaframes[frameidx].next_free_or_dirty = oldhead;
    }
    else {
      manager->writeback = &manager->metaframes[frameidx];
      manager->metaframes[frameidx].next_free_or_dirty = NULL;
    }
  }
  else if (manager->metaframes[frameidx].state == FS_PINNED){
    manager->metaframes[frameidx].state = FS_UNPINNED;

    // hashtable removal
    page_index *pageidxptr = calloc(1, sizeof(page_index));
    *pageidxptr = manager->metaframes[frameidx].index;
    gboolean remove_key = g_hash_table_remove(manager->lookup_table, pageidxptr);
    if (remove_key != true) {
      free(pageidxptr);
    }
    
    if (manager->freelist != NULL){
      meta_frame *oldhead = manager->freelist;
      manager->freelist = &manager->metaframes[frameidx];
      manager->metaframes[frameidx].next_free_or_dirty = oldhead;
    }
    else {
      manager->freelist = &manager->metaframes[frameidx];
      manager->metaframes[frameidx].next_free_or_dirty = NULL;      
    }
  }
  else{
    // if we are trying to unpin a page on a frame that isn't in either of those two states,
    // there is most likely an issue
    return STATUS_ERR;
  }
  return STATUS_OK;
}

/*
  this function goes to the backing store to allocate a new page, and then
  pin it into the buffer, giving the user both a page index and frame pointer.
 */
status buff_alloc_frame(buffer_manager *manager, page_frame_pair *page_frame)
{
  if (manager == NULL){
    return STATUS_NO_MANAGER;
  }
  if (page_frame == NULL){
    return STATUS_ERR;
  }

  // first, we get a page from the store.
  page_index page = ERR_SET;
  status pagealloc = alloc_page(NULL, &page, manager->store);
  if (pagealloc != STATUS_OK){
    return pagealloc;
  }
  
  // now, we can pin it to the buffer.
  frame *frame = NULL;
  status framepin = buff_pin(manager, &frame, page);
  if (framepin != STATUS_OK){
    return framepin;
  }

  // set the bundle, and return.
  page_frame->frame = frame;
  page_frame->page = page;
  
  return STATUS_OK;
}

/*
  unpin the given frame, and free it's underlying page on disk.
 */
status buff_free_frame(buffer_manager *manager, frame *frame){
  if (manager == NULL){
    return STATUS_NO_MANAGER;
  }
  if (frame == NULL){
    return STATUS_ERR
  }
  // if the frame is freed, but is dirty, it's no longer dirty.
  // call buff_unpin
  // call free_page
  return STATUS_OK;
}

/*
  this function is used to mark frame/page pairs as dirty,
  this works for all page types except FS_UNPINNED.
 */
status buff_mark_page(buffer_manager *manager, frame *frame)
{
  if (manager == NULL) {
    return STATUS_NO_MANAGER;
  }
  frame_index frameidx = get_frame_index_from_frame(manager, frame);

  if (manager->metaframes[frameidx].state == FS_PINNED){
    manager->metaframes[frameidx].state = FS_PINNED_DIRTY;
  }
  else if (manager->metaframes[frameidx].state == FS_PINNED_DIRTY){
    // we are already here!
  }
  else {
    return STATUS_MARK_ERR;
  }
  
  return STATUS_OK;
}

/*
  take all currently dirty pages being held in the buffer manager, and write
  them to disk.
 */
TODO("`buff_flush_all()`- update the writeback list after flush")
status buff_flush_all(buffer_manager *manager)
{
  // if there is no manager, we are done here.
  if (manager == NULL){
    return STATUS_NO_MANAGER;
  }

  // for every dirty frame we have, we need to write it back to the disk store.
  for (meta_frame *meta = manager->writeback; meta != NULL; meta = meta->next_free_or_dirty){
    status put = put_page(manager->buffer[get_frame_index_from_meta(manager, meta)], manager->store, meta->index);
    if (put == STATUS_OK) {
      meta->state = FS_UNPINNED;
    }
  }
  return STATUS_OK;
}

TODO("'sync_frame()'- write function for single frame update to disk.")
