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

/*
  configuration structure
 */
typedef struct btree_config {
  TODO("`btree_config`- fill out and create parameters")
  buffer_manager *manager;
  page_frame_pair root;


} btree_config;

/*
  tree components types
  all of the data about each of the nodes in the tree are stored
  in the pages which the `btree_node` points to.
 */
typedef frame btree;
typedef frame btree_node;

/*
  internal node structures
  these structures are used strictly for casting on top of pages.
  they should never be allocated!
 */
typedef struct int_btree_node {
  TODO("`int_btree_node`- create fields for casting")
  page_type type;

  // the rest of the bytes on the page.
  char bytes[PAGESIZE - (
			 sizeof(page_type)
			 )];
} int_btree_node;
TODO("btree.h- fix drawing")
/*
  the pointers to each part of any node can be represented as an array
  of `uint_16t`s. "pointing" to the other side of the page

 data -> +----------------------+---------------------------+--------+
	 |             	       	|  	     	            |  	     |
	 |      blah            |        something          |  	     |
	 +----------------------+---------------------------+        |
	 |                                   	 	 	     |
	 |                                   	 	 	     |
	 |					 	 	     |
	 |                                       	 	     |
	 |						 	     |
	 |						 	     |
	 |		                          	 	     |
	 |        +-----------+------------+-----------+-------------+
	 |	  | 	      |		   |	       |             |
	 |        |      0    |    0       |    256    |    type     |
	 +--------+-----------+------------+-----------+-------------+ <- data/dir offset pointers

  both sides of the page grow toward each other, 
*/

/*
  internal btree functions
 */
status btree_create_node(btree_config *config, page_index page, page_type type, btree_node **node);


/*
  btree functions
 */
status btree_create(btree_config *config);
status btree_destroy(btree tree);

status btree_insert();
status btree_remove();

#endif
