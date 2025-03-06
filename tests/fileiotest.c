/*
 * Copyright 2025 Nick Brown <njbrown4@buffalo.edu>
 */

#include <stdio.h>
#include <string.h>

#include "../src/lib/fileio.h"


int main(int argc, char *argv[]){
  
  backing *files = create_new_backing("testfilename");
  if (files == NULL) {
    printf("could not create files! structure was NULL");
    return -1;
  }
  printf("\nprinting file descriptors:\n");
  printf("metafd: %i\n", files->metafd);
  printf("storefd: %i\n", files->storefd);

  printf("\n closing filestore...\n");
  close_backing(files);
  files = NULL;

  
  files = open_backing("testfilename");
  if (files == NULL) {
    printf("could not open files! structure was NULL");
    return -1;
  }
  printf("\nprinting file descriptors:\n");
  printf("metafd: %i\n", files->metafd);
  printf("storefd: %i\n", files->storefd);


  printf("\n closing filestore...\n");
  close_backing(files);
  files = NULL;

  if (remove_backing("testfilename") == 0) {
    printf("files removed!\n");
  }
  else {
    printf("files could not be removed.!");
    return -1;
  }
  
  
  return 0;
}
