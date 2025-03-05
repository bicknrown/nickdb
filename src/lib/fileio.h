/*
 * Copyright 2025 Nick Brown <njbrown4@buffalo.edu>
 */

#include "./datastructures.h"

// backing
backing *create_new_backing_files(char *name);
backing *open_backing_store(char *name);
void close_backing_store(backing *files);
int remove_backing_store(char *filename);

// pages
int get_page(void *dest, backing *file, int index);
int put_page(void *src, backing *file, int index);
