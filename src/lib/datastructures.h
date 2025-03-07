/*
 * Copyright 2025 Nick Brown <njbrown4@buffalo.edu>
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
