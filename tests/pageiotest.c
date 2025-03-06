/*
 * Copyright 2025 Nick Brown <njbrown4@buffalo.edu>
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "../src/lib/fileio.h"

#define PAGESIZE 4096

int main(int argc, char *argv[]){
  
  backing *file = create_new_backing("teststore");
  if (file == NULL) {
    printf("could not create files! structure was NULL");
    return -1;
  }
  printf("\nprinting file descriptors:\n");
  printf("storefd: %i\n", file->storefd);

  // testing paging.

  void *src = calloc(1, PAGESIZE);
  memset(src, 65, PAGESIZE);
  
  //void *dest = calloc(1, PAGESIZE);

  if (put_page(src, file, 0) != 0) {
    printf("\ncould not get page!\n");
    return -1;
  }

  printf("\n closing filestore...\n");
  close_backing(file);
  file = NULL;

  if (remove_backing("testfilename") == 0) {
    printf("files removed!\n");
  }
  else {
    printf("files could not be removed.!");
    return -1;
  }
  
  
  return 0;
}
