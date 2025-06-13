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

#include "btree.h"

/*
  compare two keys of a given length. if `comparator` is set to NULL,
  the `memcmp` function is used.
  
  the function returns an integer less than, equal to, or greater than
  zero if the first `len` bytes  of `first`  is  found,  respectively,
  to be less than, to match, or be greater than the first `len` bytes of second.
  if `len` is zero, the return value is zero.
 */
int32_t btree_default_cmp_keys(void *first, void *second, size_t len){
  int return_value = 0;
  if (len == 0){
    return return_value;
  }
  // the pointer should not be null, but this seems like a reasonable way
  // to handle it.
  if ((first == NULL) | (second == NULL)){
    return return_value;
  }

  // default is to use memcmp, very nice.
  return_value = memcmp(first, second, len);

  return return_value;
}

/*
  create a node of the given type from a frame given by
  the buffer manager, and backed by a page on disk.
 */
status btree_alloc_node(btree_config *config, page_type type, btree_node **node)
{
  if (config == NULL){
    return STATUS_NO_CONFIG;
  }
  if ((type != DIR_PAGE) && (type != DATA_PAGE)){
    return STATUS_ERR;
  }

  // allocate the area to place the new node.
  page_frame_pair page_frame;
  page_frame.frame = NULL;
  page_frame.page = ERR_SET;
  status frame_alloc = buff_alloc_frame(config->manager, &page_frame);
  if (frame_alloc != STATUS_OK){
    return frame_alloc;
  }

  // set the frame location.
  *node = page_frame.frame;
  
  ((int_btree_node *)node)->type = type;
  
  return STATUS_OK;
}
/*
  free the given node, assuming it has already
  been removed from the tree.
 */
status btree_free_node(btree_config *config, btree_node *node)
{
  if (config == NULL){
    return STATUS_NO_CONFIG;
  }
  if (node == NULL){
    return STATUS_OK;
  }

  // unpin the frame that the node was using.
  status node_free = buff_free_frame(config->manager, node); 
  if (node_free != STATUS_OK) {
    return node_free;
  }
  
  return STATUS_OK;
}

TODO("split leaf")
TODO("fill data(leaf) page until full")
TODO("later... split directory.")



/*

 */
status btree_create(btree_config *config,
		    buffer_manager *manager,
		    page_index root,
		    size_t key_size,
		    btree_cmp_keys *comparator
		    )
{
  // check for all the important things.
  if (config == NULL){
    return STATUS_NO_CONFIG;
  }
  else if (manager == NULL){
    return STATUS_NO_MANAGER;
  }
  else if (root < LOWEST_PAGE ||
	   key_size < SMALLEST_KEY_SIZE){
    return STATUS_ERR;
  }

  // set the fields.
  config->manager = manager;
  config->key_size = key_size;
  config->root.page = root;

  // set the default key comparator if the user doesn't supply one.
  if (comparator == NULL){
    config->comparator = btree_default_cmp_keys;
  }
  else {
    config->comparator = comparator;
  }

  // pin the root node and get the frame location back, then store it.
  frame *root_node = NULL;
  status pin_root = buff_pin(config->manager, &root_node, root);
  if (pin_root != STATUS_OK){
    return pin_root;
  }
  config->root.frame = root_node;

  /*
    next steps:
    - does the create need a root page argument?
    - setup the root node as a dir page.
    - allocate an empty leaf node and attach it to the root.
   */
  
  return STATUS_OK;
}

/*
  
 */
status btree_open(btree_config *config,
		  buffer_manager *manager,
		  page_index root,
		  size_t key_size,
		  btree_cmp_keys *comparator
		  )
{
  // check for all the important stuff.
  if (config == NULL){
    return STATUS_NO_CONFIG;
  }
  else if (manager == NULL){
    return STATUS_NO_MANAGER;
  }
  else if (root < LOWEST_PAGE ||
	   key_size < SMALLEST_KEY_SIZE){
    return STATUS_ERR;
  }

  // set the fields.
  config->manager = manager;
  config->key_size = key_size;
  config->root.page = root;

  // set the default key comparator if the user doesn't supply one.
  if (comparator == NULL){
    config->comparator = btree_default_cmp_keys;
  }
  else {
    config->comparator = comparator;
  }

  // pin the root node and get the frame location back, then store it.
  frame *root_node = NULL;
  status pin_root = buff_pin(config->manager, &root_node, root);
  if (pin_root != STATUS_OK){
    return pin_root;
  }
  config->root.frame = root_node;

  /*
    next steps:
    - open def needs the root page.
    - no need to configure the storage because it should already be a tree.
    - figure out what else open needs.
   */
  
  return STATUS_OK;
}

/*

 */
TODO("`btree_destroy()`- everything")
status btree_destroy(btree tree)
{
  tree = tree + 1; // remove error of unused parameter.
  
  return STATUS_OK;
}

/*

 */int32_t btree_cmp_keys(void *first, void *second, size_t len, void *comparator){
  int return_value = 0;
  if (len == 0){
    return return_value;
  }
  // the pointer should not be null, but this seems like a reasonable way
  // to handle it.
  if ((first == NULL) | (second == NULL)){
    return return_value;
  }


TODO("`btree_insert()`- everything")
status btree_insert()
{

  return STATUS_OK;
}

/*

 */
TODO("`btree_remove()`- everything")
status btree_remove()
{

  return STATUS_OK;
}
