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
#include "constants.h"

/*
  something
 */

typedef enum frame_state {
  FS_PINNED,
  FS_UNPINNED,
  FS_PINNED_DIRTY,
  FS_UNPINNED_DIRTY,
} frame_state;

typedef char frame[PAGESIZE];

typedef struct meta_frame {
  frame_state state; // 1 = pinned
  int page_id;
  // if free or dirty, this next pointer is now useful.
  struct meta_frame *next_free_or_dirty;
} meta_frame;

/*
  Each buffer manager will have it's own state stored inside a struct that looks
  like the following. this structure is allocated and returned by buff_create()
  and freed by buff_destroy()
 */
typedef struct buffer_manager {
  int frames;
  // a contagious region which is `frames * PAGESIZE` bytes large,
  // can be treated like an array of frames.
  frame *buffer;

  // also a contiguous region, with a one to one mapping to the buffer array.
  // can be treated like an array of `meta_frame`s.
  meta_frame *metaframes;

  // a linked list of `meta_frame`s which point to the next free frame, if any.
  // if the page is free, and the next is NULL, there are no more free frames.
  // this list must contain every frame in the state `FS_UNPINNED`.
  // NULL on initialization.
  meta_frame *freelist;

  // unpinned dirty pages to write back to disk.
  // this list must contain every frame in the state `FS_UNPINNED_DIRTY`.
  // NULL on initialization.
  meta_frame *writeback;
  
} buffer_manager;

buffer_manager *buff_create(char *name, int frames);
void *buff_pin(buffer_manager *man, int page_index);
int buff_unpin(buffer_manager *man, void *frame);
int buff_mark_page(buffer_manager *man, void *frame);
int buff_flush_all(buffer_manager *man);
int buff_destroy(buffer_manager *man);


/*
  enum of page types:
  each page has a header, which is the page type.
  the page header is 4 bytes.
  - metadata (magic byte)
  - freepage

  struct page type {
    // 4 byte int.
  }
 */
/*
  remove allocations with stack arrays?
  clear with bzero(3)
 */

/*
  within a give page, one abstraction above.
  a structure to handle reserved space size, and record size:
  something like

  struct page_spec {
    4 byte int record_size;
    "" number_of_records_used;
    "" reserved_bytes
  }
  struct page_spec { // page specification
    int reserved_size; // 4 bytes
    int record_size;
  }
  void *fixed_get(void *page, int record_index);
 */

/*
  void *variable
 */
/*
  nevermind, buffermanager

  typedef frame 
  
  struct meta_frame {
  
  }
  
  struct buffer_manager {
    int frames;
    frame *start; // start of frame memory

    meta_frame *start // metadata for frames.
  
  }
  
 */
