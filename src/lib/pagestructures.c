/*
 * Copyright 2025 Nick Brown <njbrown4@buffalo.edu>
 */

#include <sys/types.h>

#include "pagestructures.h"



/* `P`age `A`llocation `T`able abstraction.
   ***THIS STRUCTURE TAKES UP A WHOLE 4KB PAGE***
   
   the `version` character refers to which canonical table
   we are working with.
   it is a single byte, with an extra padding byte for even division.
   
   `tableA` refers to the offset into the first backing store.
   'tableB' refers to the offset into the second backing store.
   both tables are 2047 bytes, respectively.
   `version` + `padding` + `tableA` + `tableB` = 4096 bytes, a full page.
*/
struct PAT {
  char version;
  // this padding is purely for convenience and reminder,
  // the calculation would account for this regardless.
  char padding;

  // the byte offsets of the two page table versions.
  // these are NOT pointers.
  u_int32_t tableA;
  u_int32_t tableB;
};


