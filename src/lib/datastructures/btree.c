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
  
 */
status btree_create_node(btree_config *config, page_index page, page_type type, btree_node **node){
  status root_pin = buff_pin(config->manager, node, page);
  if (root_pin != STATUS_OK){
    return STATUS_PIN_ERR;
  }
  switch (type)
    {
    case DIR_PAGE:
      ((int_btree_node *)node)->type = type;
      
      break;
    case DATA_PAGE:
      ((int_btree_node *)node)->type = type;
      /*
	depending on the record size, the amount of records that can be stored will change.
	there also has to be enough room for pointers/offsets.

	something like:

	pointer space = ((PAGE_SIZE / record_size) * sizeof(short int))

	max records = (PAGE_SIZE / record_size) 
	
       */
      
      break;
    default:
      // if the type is just wrong, then we just leave.
      return STATUS_ERR;
    }
  
  return STATUS_OK;
}

/*

 */
TODO("`btree_create()`- everything")
status btree_create(btree_config *config){
  if (config == NULL){
    return STATUS_NO_MANAGER;
  }
  

  
  return STATUS_OK;
}

/*

 */
TODO("`btree_destroy()`- everything")
status btree_destroy(btree tree){
  tree = tree + 1; // remove error of unused parameter.
  
  return STATUS_OK;
}

/*

 */
TODO("`btree_insert()`- everything")
status btree_insert(){

  return STATUS_OK;
}

/*

 */
TODO("`btree_remove()`- everything")
status btree_remove(){

  return STATUS_OK;
}
