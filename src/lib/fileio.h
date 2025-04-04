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

#define STOREEXTLEN 6
#define METAEXTLEN 5
#define NULLLEN 1

#define METAEXT ".meta"
#define STOREEXT ".store"

typedef struct backing {
  int metafd;
  int storefd;
} backing;

/*
  this structure lives in both the metadata page, and each free page.
  the offset corresponds to the next free page.
  if the offset is -1, there are no more free pages.
*/
typedef struct freepage {
  int offset; // replace with index, then do calc.
} freepage;

/*
  .this structure defines the layout for the first page of
  the storage backing.
*/
typedef struct meta_page {
  // the current size of the backing, in pages.
  int size;
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
backing *create_new_backing(char *name);
backing *open_backing(char *name);
void close_backing(backing *files);
int remove_backing(char *filename);

// page helpers
int index_to_offset(int page_index);
int offset_to_index(int offset);

// pages
int alloc_page(void *src, backing *file);
int free_page(backing *file, int page_index);
int get_page(void *dest, backing *file, int page_index);
int put_page(void *src, backing *file, int page_index);


