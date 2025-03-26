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

#include "string.h"

#include "./buffer_manager.h"
#include "./fileio.h"

buffer_manager *buff_create(char *name, int frames)
{
  //
  return NULL;
}

void *buff_pin(buffer_manager *man, int page_index)
{
  //
  return NULL;
}

int buff_unpin(buffer_manager *man, void *frame)
{
  //
  return -1;
}

int buff_mark_page(buffer_manager *man, void *frame)
{
  //
  return -1;
}

int buff_flush_all(buffer_manager *man)
{
  //
  return -1;
}

int buff_destroy(buffer_manager *man)
{
  //
  return -1;
}
