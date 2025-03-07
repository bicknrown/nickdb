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
