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

#ifndef BTREEH
#define BTREEH

#include "../constants.h"
#include "../buffer_manager.h"
#include <bits/stdint-intn.h>

/*
  types for configuration.
 */
typedef int32_t btree_cmp_keys(void *first, void *second, size_t len);

/*
  configuration structure
 */
typedef struct btree_config {
  buffer_manager *manager;
  page_index root;
  size_t key_size;
  btree_cmp_keys *comparator;

} btree_config;

/*
  tree components types
  all of the data about each of the nodes in the tree are stored
  in the pages which the `btree_node` points to.
 */
typedef frame btree;
typedef frame btree_node;

typedef page_index dir;
typedef void separator;

typedef int16_t record;
typedef void data;

/*
  internal node structures
  these structures are used strictly for casting on top of pages.
  they should never be allocated!
 */
typedef struct int_btree_node {
  page_type type;

  // the rest of the bytes on the page.
  char bytes[PAGESIZE - (
			 sizeof(page_type)
			 )];
} int_btree_node;

/*
  internal btree functions
 */
int64_t btree_get_key_sep_size(void *key);

int32_t btree_default_cmp_keys(void *first, void *second, size_t len);

dir *btree_get_dir_list(btree_node *node);
separator *btree_get_sep_list(btree_node *node);

record* btree_get_record_list(btree_node *node);
void *btree_get_record_data(btree_node *node, record number);

status btree_insert_sep_value(btree_node *node, separator *sep);
status btree_insert_record_value(btree_node *node, record number, void *data, size_t length);

/*
  btree functions
 */
status btree_create(btree_config *config,
		    buffer_manager *manager,
		    size_t key_size,
		    btree_cmp_keys *comparator
		    );
status btree_open(btree_config *config,
		  buffer_manager *manager,
		  page_index root,
		  size_t key_size,
		  btree_cmp_keys *comparator
		  );
status btree_destroy(btree tree);

status btree_alloc_node(btree_config *config, page_type type, btree_node **node);
status btree_free_node(btree_config *config, btree_node *node);
page_index btree_get_root_id(btree_config *config);

status btree_insert(btree_config *config, void *key, data *value, size_t value_length);
status btree_remove(btree_config *config);

status btree_get(btree_config *config, void *key, data **value);

#endif
