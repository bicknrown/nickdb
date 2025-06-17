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

#ifndef CONSTANTSH
#define CONSTANTSH

#include <stdio.h>
#include <stdbool.h>

#define DO_PRAGMA(x) _Pragma (#x)
#define TODO(x) DO_PRAGMA(message ("TODO - " #x))

#define PAGESIZE 4096

#define STOREEXTLEN 6
#define NULLLEN 1

#define ERR_SET -1

#define EMPTY_LIST -1

#define STOREEXT ".store"

#define METADATA 0
#define LOWEST_PAGE 1
#define SMALLEST_KEY_SIZE 1

#define ARR_OFFSET 1

/*
  types and states.
 */

typedef ssize_t page_index;
typedef ssize_t byte_offset;

typedef ssize_t frame_index;

typedef char frame[PAGESIZE];

typedef enum page_type{
  FREE_PAGE,
  DATA_PAGE,
  DIR_PAGE,
} page_type;

typedef enum frame_state {
  FS_PINNED,
  FS_UNPINNED,
  FS_PINNED_DIRTY,
  FS_UNPINNED_DIRTY,
} frame_state;

typedef struct page_frame_pair {
  page_index page;
  frame *frame;  
} page_frame_pair;


/*
  a not awful way to emulate errno functionality with a compile time check.
  https://stackoverflow.com/a/58500930
 */

typedef enum status {
  STATUS_OK = 0,
  STATUS_ERR,

  STATUS_NO_BACKING,
  STATUS_NO_FILE,
  STATUS_BAD_FILE,
  STATUS_BAD_OPEN,
  STATUS_BAD_CLOSE,
  STATUS_BAD_REMOVE,
  
  STATUS_BAD_READ,
  STATUS_BAD_WRITE,
  STATUS_BAD_SEEK,

  STATUS_META_PAGE,

  STATUS_NO_MANAGER,

  STATUS_NO_FREE_FRAMES,
  STATUS_PIN_ERR,
  STATUS_UNPIN_ERR,
  STATUS_MARK_ERR,
  STATUS_FLUSH_ERR,

  STATUS_NO_CONFIG,
  
  STATUS_TEST_RESET,
  STATUS_TEST_ERR,

  /* the final frontier*/
  STATUS_MAX,
} status;

extern const char * const status_str[];

#endif
