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

typedef struct backing {
  int metafd;
  int storefd;
}backing;

/*
  this structure lives in both the metadata page, and each free page.
  the offset corresponds to the next free page.
  if the offset is -1, there are no more free pages.
*/
typedef struct freepage {
  int offset;
}freepage;

/*
  .this structure defines the layout for the first page of
  the storage backing.
*/
typedef struct meta_page {
  // the current size of the backing, in KB.
  int size;
  freepage freelist_head;
  freepage freelist_tail;

}meta_page;
