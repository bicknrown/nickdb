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

#include "../src/lib/fileio.h"

int main(int argc, char *argv[]){
  
  backing *file = calloc(1, sizeof(backing));
  status file_status= create_new_backing("testfilename", file);
  if (file_status != STATUS_OK) {
    fprintf(stderr, "could not create files! status was NOT OK");
    return STATUS_TEST_ERR;
  }
  fprintf(stderr, "\nprinting file descriptor:\n");
  fprintf(stderr, "storefd: %i\n", file->storefd);

  fprintf(stderr, "\n closing filestore...\n");
  close_backing(file);
  free(file);
  file = NULL;
  file_status = STATUS_TEST_RESET;

  
  file = calloc(1, sizeof(backing));
  file_status = open_backing("testfilename", file);
  if (file_status != STATUS_OK) {
    fprintf(stderr, "could not create files! status was NOT OK");
    return STATUS_TEST_ERR;
  }
  fprintf(stderr, "\nprinting file descriptor:\n");
  fprintf(stderr, "storefd: %i\n", file->storefd);

  fprintf(stderr, "\n closing filestore...\n");
  close_backing(file);
  free(file);
  file = NULL;
  file_status = STATUS_TEST_RESET;

  file_status = remove_backing("testfilename");
  if (file_status != STATUS_OK) {
    fprintf(stderr, "files could not be removed.!");
    return STATUS_BAD_REMOVE;
  }
  else {
    fprintf(stderr, "files removed!\n");
  }
  
  return STATUS_OK;
}
