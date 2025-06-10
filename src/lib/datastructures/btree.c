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
#include <string.h>


/*
  create a node of the given type from a frame given by
  the buffer manager, and backed by a page on disk.
 */
status btree_alloc_node(btree_config *config, page_type type, btree_node **node)
{
  if (config == NULL){
    return STATUS_NO_CONFIG;
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

  switch (type)
    {
    case DIR_PAGE:
      ((int_btree_node *)node)->type = type;
      
      break;
    case DATA_PAGE:
      ((int_btree_node *)node)->type = type;
      
      break;
    default:
      // if the type is just wrong, then we just leave.
      return STATUS_ERR;
    }
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
  // clear the node.
  memset(node, 0, PAGESIZE);
  // unpin the frame that the node was using.
  status node_unpin = buff_unpin(config->manager, (frame *)node);
  if (node_unpin != STATUS_OK) {
    return node_unpin;
  }
  
  return STATUS_OK;
}

TODO("split leaf")
TODO("fill data(leaf) page until full")
TODO("later... split directory.")

/*

 */
TODO("`btree_create()`- everything")
status btree_create(btree_config *config)
{
  if (config == NULL){
    return STATUS_NO_CONFIG;
  }
  

  
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

 */
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
