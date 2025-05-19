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

// being used for hashtable only.
#include <stdbool.h>
#include <glib.h>

#include "fileio.h"

typedef enum frame_state {
  FS_PINNED,
  FS_UNPINNED,
  FS_PINNED_DIRTY,
  FS_UNPINNED_DIRTY,
} frame_state;

typedef char frame[PAGESIZE];

typedef ssize_t frame_index;

typedef struct meta_frame {
  frame_state state;
  page_index index;
  // if free or dirty, this next pointer is now useful.
  struct meta_frame *next_free_or_dirty;
} meta_frame;

/*
  Each buffer manager will have it's own state stored inside a struct that looks
  like the following. this structure is allocated and returned by buff_create()
  and freed by buff_destroy()
 */
typedef struct buffer_manager {
  backing *store;
  size_t frames;
  // a contagious region which is `frames * PAGESIZE` bytes large,
  // can be treated like an array of frames.
  frame *buffer;

  // also a contiguous region, with a one to one mapping to the buffer array.
  // can be treated like an array of `meta_frame`s.
  meta_frame *metaframes;

  // a linked list of `meta_frame`s which point to the next free frame, if any.
  // if the page is free, and the next is NULL, there are no more free frames.
  // this list must contain every frame in the state `FS_UNPINNED`.
  meta_frame *freelist;

  // unpinned dirty pages to write back to disk.
  // this list must contain every frame in the state `FS_UNPINNED_DIRTY`.
  // NULL on initialization.
  meta_frame *writeback;

  // a hashtable which keeps track of page ids and frame ids.
  GHashTable *lookup_table;
  
} buffer_manager;

// helpers
frame_index get_frame_index(buffer_manager *manager, meta_frame *meta);



// buffer manager
status buff_create(char *storename, buffer_manager **manager, int frames);
status buff_destroy(buffer_manager **manager);
status buff_pin(buffer_manager *manager, frame *pinned, page_index index);
status buff_unpin(buffer_manager *manager, void *frame);
status buff_mark_page(buffer_manager *manager, void *frame);
status buff_flush_all(buffer_manager *manager);
