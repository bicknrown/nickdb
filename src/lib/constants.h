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

#define PAGESIZE 4096

#define STOREEXTLEN 6
#define NULLLEN 1

#define ERR_SET -1

#define EMPTY_LIST -1

#define STOREEXT ".store"

#define METADATA 0

typedef enum page_type{
  FREE_PAGE,
  DATA_PAGE,
  DIR_PAGE,
} page_type;

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
  STATUS_UNPIN_ERR,
  STATUS_MARK_ERR,
  STATUS_FLUSH_ERR,
  
  STATUS_TEST_RESET,
  STATUS_TEST_ERR,

  /* the final frontier*/
  STATUS_MAX,
} status;

extern const char * const status_str[];
