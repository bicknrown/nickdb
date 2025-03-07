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
