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
#include <stdio.h>
#include <stdint.h>

#include "constants.h"

typedef ssize_t page_index;
typedef ssize_t byte_offset;

typedef struct backing {
  int storefd;
} backing;

/*
  this structure lives in both the metadata page, and each free page.
  the offset corresponds to the next free page.
  if the offset is -1, there are no more free pages.
*/
typedef struct freepage {
  ssize_t offset; // replace with index, then do calc.
} freepage;

/*
  .this structure defines the layout for the first page of
  the storage backing.
*/
typedef struct meta_page {
  // the current size of the backing, in pages.
  page_index size;
  /*
    note for future nick:
    we don't need a tail if we just treat the "free list" like a stack.
    less stuff to go wrong when you are just pushing and popping.
    in any case, what i have here does work, so just food for thought.
   */
  freepage freelist_head;
  freepage freelist_tail;

} meta_page;

// backing
status create_new_backing(char *filename, backing *store);
status open_backing(char *filename, backing *store);
status close_backing(backing *store);
status remove_backing(char *filename);

// page helpers
byte_offset index_to_offset(page_index index);
page_index offset_to_index(byte_offset offset);

// pages
status alloc_page(void *src, page_index *index, backing *store);
status free_page(backing *store, page_index index);
status get_page(void *dest, backing *store, page_index index);
status put_page(void *src, backing *store, page_index index);


