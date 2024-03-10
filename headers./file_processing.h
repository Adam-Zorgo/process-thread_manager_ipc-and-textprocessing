#ifndef FILE_PROCESSING_H
#define FILE_PROCESSING_H

#include "constants.h"

void *process_file_segment(void *arg);
void merge_and_print_char_counts(file_segment_t* segments, size_t num_segments);
int create_thread_or_process(void *(*start_routine)(void *), void *arg);
void wait_for_threads_or_processes();
void read_and_distribute_file(const char *filename);

#endif