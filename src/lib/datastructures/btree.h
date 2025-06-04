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
#ifndef CONSTANTSH
#define CONSTANTSH
#include "../constants.h"
#endif

/*
  configuration structure
 */
typedef struct btree_config {
  TODO("`btree_config`- fill out and create parameters")
} btree_config;

/*
  tree components types
  all of the data about each of the nodes in the tree are stored
  in the pages which the `btree_node` points to.
 */
typedef void btree;
typedef void btree_node;

/*
  internal node structures
  these structures are used strictly for casting on top of pages.
  they should never be allocated!
 */
typedef struct int_btree_node {
  TODO("`int_btree_node`- create fields for casting")
} int_btree_node;

/*
  btree functions
 */
status btree_create(btree_config *config, btree **tree);
status btree_destroy(btree **tree);

status btree_insert();
status btree_remove();
