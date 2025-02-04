/*
 * Copyright 2025 Nick Brown <njbrown4@buffalo.edu>
 */

#include "./datastructures.h"

int create_new_backing_store();
int open_backing_store(char* filename);
void close_backing_store(backing* store);
int remove_backing_store(char* filename);
