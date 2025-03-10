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
  
  buffer_manager *buff_init(char *name, int frames);
  void *buff_pin(buffer_manager *man, int page_index);
  int buff_unpin(buffer_manager *man, void *frame);
  int buff_mark_page(buffer_manager *man, void *frame);
  int buff_flush_all(buffer_manager *man);
  int buff_destroy(buffer_manager *man);
 */
