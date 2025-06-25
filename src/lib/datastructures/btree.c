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
  given a pointer to a key, which ends with the NUL value `\0`,
  return the length of the key in bytes, not including the NUL byte.
  returns ERR_SET in error
 */
int64_t btree_get_key_sep_size(void *key)
{
  int64_t length = 0;
  if (key == NULL) {
    return ERR_SET;
  }

  // count the number of bytes before the NUL.
  for (/* ;) */; ((char *)key)[length] != '\0'; length++)
    ;
  
  return length;
}

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
  given a node of the type `DIR_PAGE`, return a pointer to the list of pages.
 */
dir *btree_get_dir_list(btree_node *node)
{
  return (dir *)(((int_btree_node *)node)->bytes);
}

/*
  given a node of the type 'DIR_PAGE', return a pointer to the list of
  separator values.
 */
separator *btree_get_sep_list(btree_node *node)
{
  return &(((int_btree_node *)node)->bytes[END_OF_PAGE - ARR_OFFSET]);
}

/*
  given a node of type `DATA_PAGE`, return a pointer to the list of records.
 */
record *btree_get_record_list(btree_node *node)
{
  return (record *)(((int_btree_node *)node)->bytes);
}

/*
  given a node of type `DATA_PAGE`, and a record, return a pointer to the data
  referred to by the record, or NULL in error.
 */
void *btree_get_record_data(btree_node *node, record number)
{
  return &(((int_btree_node *)node)->bytes[number]);
}

/*
  insert a separator value into the correct location in
  the list for the given directory node.
  
 */
status btree_insert_sep_value(btree_node *node, separator *sep)
{
  if (node == NULL || sep == NULL) {
    return STATUS_ERR;
  }
  TODO("finish insert for separator values.")
  return STATUS_OK;
}

/*
  insert record data at the supplied offset in the given data node.
 */
status btree_insert_record_value(btree_node *node, record number, void *data,
                                 size_t length)
{
  if (node == NULL || RECORD_BOUNDS(number)) {
    return STATUS_ERR;
  }
  // we can allow for empty values.
  if (data == NULL && length != 0) {
    return STATUS_ERR;
  }

  TODO("finish comparisons for inserting record values..")
  return STATUS_OK;
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
  // dereference the double pointer to set the type.
  (((int_btree_node *)*node)->type) = type;
  
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

/*
  create a btree from the supplied configuration parameters.
  `config` is a pointer to an empty btree config.
 */
status btree_create(btree_config *config,
		    buffer_manager *manager,
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
  // set the fields that we already have.
  config->manager = manager;
  config->key_size = key_size;
  
  // new tree, allocate a new root.
  btree_node *root_frame = NULL;
  status root_status = btree_alloc_node(config, DIR_PAGE, &root_frame);
  if (root_status != STATUS_OK) {
    return root_status;
  }

  // now we can set the root node in the config using the pointer to the node.
  config->root =
    config->manager->metaframes[get_frame_index_from_frame(config->manager,
									root_frame)].index;

  // set the default key comparator if the user doesn't supply one.
  if (comparator == NULL){
    config->comparator = btree_default_cmp_keys;
  }
  else {
    config->comparator = comparator;
  }

  // now we can create the empty child leaf node.
  btree_node *leaf_frame = NULL;
  status leaf_status = btree_alloc_node(config, DATA_PAGE, &leaf_frame);
  if (leaf_status != STATUS_OK) {
    return leaf_status;
  }
  page_index leaf_page =
    config->manager->metaframes[get_frame_index_from_frame(config->manager,
									leaf_frame)].index;
  // accessing the page pointer list.
  dir *root_dir_pointer_list =
    (void *)(((int_btree_node *)root_frame)->bytes);

  // accessing the separator value list
  /*separator *root_dir_separator_list =
    &(((int_btree_node *)root_frame)->bytes[(PAGESIZE - sizeof(page_type) - ARR_OFFSET)]);*/

  // now we take the empty child page index, and insert it as the first pointer,
  // with no separator values.
  root_dir_pointer_list[1] = leaf_page;

  // unpin both of the new node pages.
  status leaf_unpin = buff_unpin(config->manager, leaf_frame);
  if (leaf_unpin != STATUS_OK) {
    return leaf_unpin;
  }
  status root_unpin = buff_unpin(config->manager, root_frame);
  if (root_unpin != STATUS_OK) {
    return root_unpin;
  }
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
  config->root = root;

  // set the default key comparator if the user doesn't supply one.
  if (comparator == NULL){
    config->comparator = btree_default_cmp_keys;
  }
  else {
    config->comparator = comparator;
  }

  /*
    next steps:
    - open def needs the root page.
    - no need to configure the storage because it should already be a tree.
    - figure out what else open needs.
   */

  // unpin all pinned pages.
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
  take the given key and value, and insert them into the tree.
 */
status btree_insert(btree_config *config, void *key, void *value, size_t value_length)
{
  if (config == NULL) {
    return STATUS_NO_CONFIG;
  }
  if (key == NULL) {
    return STATUS_ERR;
  }
  if (value_length == 0 && value != NULL) {
    return STATUS_ERR;
  }

  // pin the root (directory) page.
  frame *root_frame = NULL;
  status root_pin = buff_pin(config->manager, &root_frame, config->root);
  if (root_pin != STATUS_OK || root_frame == NULL) {
    return root_pin;
  }
  // for clarity of use.
  btree_node *root_node = root_frame;

  separator *root_sep_list = btree_get_sep_list(root_node);

  // if there are no separators in the list.
  if (*(char *)root_sep_list == '\0') {
    // follow the first page pointer.
    dir *dir_list = btree_get_dir_list(root_node);
    dir first_page = dir_list[0];
    // pin the first datapage
    frame *data_frame = NULL;
    status datapage_pin = buff_pin(config->manager, &data_frame, first_page);
    if (datapage_pin != STATUS_OK) {
      return datapage_pin;
    }

    // again, for clarity.
    //btree_node first_data_node = *data_frame;
    TODO("btree_insert() - continue walking the logic.")

    
    
  }
  // if there are separators in the list.
  else {
    
  }

  /*
    - take and pin the root from the config
    - check if the separator list is empty.
      - if it is, follow the first pointer
        - pin the first datapage.
        - put the key as the first separator value
	- copy the value to the leaf, and set it's `record` pointer to
	the correct offset.
      - unpin all pages that were accessed, working backwards.
      - let's roll out!
	
      - if it is not, compare the separator value to the given key.
        - follow which ever pointer the comparison decided.
	- pin the selected datapage/dirpage.
	- if the type is another dirpage:
	  - if the separator list is empty, follow the first pointer.
	    - recurse until datapage.
	      - copy the value to the leaf, and set it's `record` pointer to
	the correct offset.
	- unpin all pages that were accessed, working backwards.
	- we are done!
	
	  - if it is not, do a comparison with the key.
	    - follow the pointer from the comparison
	    - recurse until datapage.
	      - copy the value to the leaf, and set it's `record` pointer to
	the correct offset.
	- unpin all pages that were accessed, working backwards.
	- fin.
   */
  
  return STATUS_OK;
}

/*

 */
TODO("`btree_remove()`- everything")
status btree_remove(btree_config *config)
{
  if (config == NULL) {
    return STATUS_NO_CONFIG;
  }
  return STATUS_OK;
}

TODO("split leaf, fill data(leaf) page until full. later... split directory.")
