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
inline int get_frame_index(buffer_manager *man, meta_frame *meta)
{
  return (((void *)meta) - ((void *)man->metaframes)) / sizeof(meta_frame);
}

/*
  create an area in memory of size `frames` multiplied by PAGESIZE
  to work with data coming to and from disk.
  each manager is tied to its backing store, named by `storename`

  returns NULL in error.
  
 */
buffer_manager *buff_create(char *storename, int frames)
{
  backing *store = open_backing(storename);
  if (store == NULL){
    return NULL; 
  }

  // the meta structure itself.
  buffer_manager *manager = calloc(1, sizeof(buffer_manager));
  // initial metadata
  manager->backing = store;
  manager->frames = frames;
  // the pointer for the actual buffer
  manager->buffer = calloc(frames, sizeof(frame));
  // the metadata frame buffer
  manager->metaframes = calloc(frames, sizeof(meta_frame));
  // the free list of frame starts with the the first frame.
  manager->freelist = manager->metaframes;
  // (all the frames are free!)
  for (int i=0; i < frames; i++) {
    // this should not walk of the end...
    if (i < frames - 1 ){
      manager->metaframes[i].next_free_or_dirty = &manager->metaframes[i + 1];      
    }
    else {
      manager->metaframes[i].next_free_or_dirty = NULL; 
    }
  }
  // the write-back queue is empty to start.
  manager->writeback = NULL;
  
  return manager;
}

/*
  takes in a pointer to a buffer manager, write all dirty pages to disk,
  and then free each section of allocated memory from the manager,
  including the manager itself. and close the associated backing on disk.

  returns 0 on success, -1 on error.
 */
int buff_destroy(buffer_manager *man)
{
  // if there is no manager, we are done here.
  if (man == NULL){
    return -1;
  }

  // for every dirty frame we have, we need to write it back to the disk store.
  for (meta_frame *meta = man->writeback; meta != NULL; meta = meta->next_free_or_dirty){
    int put = put_page(man->buffer[get_frame_index(man, meta)], man->backing, meta->page_index);
    if (put == 0) {
      meta->state = FS_UNPINNED;
    }
  }

  // i'm freeeeeeeee!
  free(man->metaframes);
  free(man->buffer);

  close_backing(man->backing);

  free(man);
  // free fallin'!
  
  return 0;
}

void *buff_pin(buffer_manager *man, int page_index)
{
  //
  return NULL;
}

int buff_unpin(buffer_manager *man, void *frame)
{
  //
  return -1;
}

int buff_mark_page(buffer_manager *man, void *frame)
{
  //
  return -1;
}

int buff_flush_all(buffer_manager *man)
{
  //
  return -1;
}
