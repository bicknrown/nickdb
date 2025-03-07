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

#include "./datastructures.h"

// backing
backing *create_new_backing(char *name);
backing *open_backing(char *name);
void close_backing(backing *files);
int remove_backing(char *filename);

// page helpers
int index_to_offset(int index);
int offset_to_index(int offset);

// pages
int alloc_page(void *src, backing *file);
int get_page(void *dest, backing *file, int index);
int put_page(void *src, backing *file, int index);
