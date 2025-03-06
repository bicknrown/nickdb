/*
 * Copyright 2025 Nick Brown <njbrown4@buffalo.edu>
 */

#include "./datastructures.h"

// backing
backing *create_new_backing(char *name);
backing *open_backing(char *name);
void close_backing(backing *files);
int remove_backing(char *filename);

// page helpers
int index_to_offset(int index);

// pages
int get_page(void *dest, backing *file, int index);
int put_page(void *src, backing *file, int index);
