/*
 * Copyright 2025 Nick Brown <njbrown4@buffalo.edu>
 */

#include <stdio.h>
#include <string.h>

#include "lib/fileio.h"


int main(int argc, char *argv[]){

  int backing = create_unique_backing_store();
  printf("hello there\n");
  
  return 0;
}
