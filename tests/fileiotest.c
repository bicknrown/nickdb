/*
 * Copyright 2025 Nick Brown <njbrown4@buffalo.edu>
 */

#include <stdio.h>
#include <string.h>

#include "../src/lib/fileio.h"


int main(int argc, char *argv[]){
  
  backing *files = create_new_backing_files("testfilename");
  if (files == NULL) {
    printf("could not create files! structure was NULL");
    return -1;
  }
  printf("\nprinting file descriptors:\n");
  printf("metafd: %i\n", files->metafd);
  printf("storefd: %i\n", files->storefd);

  printf("\n closing filestore...\n");
  close_backing_store(files);
  files = NULL;

  
  files = open_backing_store("testfilename");
  if (files == NULL) {
    printf("could not open files! structure was NULL");
    return -1;
  }
  printf("\nprinting file descriptors:\n");
  printf("metafd: %i\n", files->metafd);
  printf("storefd: %i\n", files->storefd);


  printf("\n closing filestore...\n");
  close_backing_store(files);
  files = NULL;
  
  return 0;
}
