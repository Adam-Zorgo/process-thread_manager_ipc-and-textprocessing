#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include <sys/resource.h>
#include <sys/time.h>
#include <pthread.h>
#include <unistd.h>
#include "../headers/file_processing.h"
#include "../headers/thread_management.h"

void *process_file_segment(void *arg) {
    file_segment_t *segment = (file_segment_t *)arg;
    for (size_t i = 0; i < segment->size; i++) {
        segment->data[i] = toupper(segment->data[i]);
        if (isalpha(segment->data[i])) {
            segment->char_count[(int)segment->data[i]]++;
        }
    }
    return NULL;
}

void merge_and_print_char_counts(file_segment_t* segments, size_t num_segments) {
    int* total_char_count = calloc(256, sizeof(int));
    if (total_char_count == NULL) {
        perror("calloc");
        return;
    }
    for (size_t i = 0; i < num_segments; i++) {
        for (int j = 0; j < 256; j++) {
            total_char_count[j] += segments[i].char_count[j];
        }
    }
    printf("Character count:\n");
    for (int i = 0; i < 256; i++) {
        if (total_char_count[i] > 0) {
            printf("%c: %d\n", i, total_char_count[i]);
        }
    }
    free(total_char_count);
}

int create_thread_or_process(void *(*start_routine)(void *), void *arg) {
    pthread_t thread_id;
    int thread_ret = pthread_create(&thread_id, NULL, start_routine, arg);
    if (thread_ret != 0) {
        printf("Error creating thread: %s\n", strerror(thread_ret));
        return -1;
    }
    return 0;
}

void wait_for_threads_or_processes() {
    for (int i = 0; i < num_processes; i++) {
        for (int j = 0; j < processes[i].num_threads; j++) {
            pthread_join(processes[i].thread_ids[j], NULL);
        }
    }
}

#ifndef O_RDONLY
#define O_RDONLY 0
#endif

void read_and_distribute_file(const char *filename) {
      FILE *file = fopen(filename, "r");
      if (file == NULL) {
          perror("fopen");
          return;
      }

      int fd = open(filename, O_RDONLY);
      if (fd == -1) {
          perror("open");
          fclose(file);
          return;
      }

      char *buffer = malloc(MAX_FILE_SIZE);
      if (buffer == NULL) {
          perror("malloc");
          close(fd);
          fclose(file);
          return;
      }

      ssize_t bytes_read = read(fd, buffer, MAX_FILE_SIZE);
      if (bytes_read == -1) {
          perror("read");
          free(buffer);
          close(fd);
          fclose(file);
          return;
      }

      close(fd);

      size_t num_segments = (bytes_read + SEGMENT_SIZE - 1) / SEGMENT_SIZE;
      file_segment_t *segments = malloc(num_segments * sizeof(file_segment_t));
      if (segments == NULL) {
          perror("malloc");
          free(buffer);
          fclose(file);
          return;
      }

      for (size_t i = 0; i < num_segments; i++) {
          segments[i].data = buffer + i * SEGMENT_SIZE;
          segments[i].size = (i == num_segments - 1) ? bytes_read % SEGMENT_SIZE : SEGMENT_SIZE;
          segments[i].char_count = calloc(256, sizeof(int));
          if (segments[i].char_count == NULL) {
              perror("calloc");
              free(buffer);
              fclose(file);
              for (size_t j = 0; j < i; j++) {
                  free(segments[j].char_count);
              }
              free(segments);
              return;
          }
      }
      clock_t start_time = clock();
      for (size_t i = 0; i < num_segments; i++) {
          // Creating a new thread or process and pass the file_segment_t as an argument
          create_thread_or_process(process_file_segment, &segments[i]);
      }
      wait_for_threads_or_processes();
  
      clock_t end_time = clock();
      double cpu_time_used = ((double) (end_time - start_time)) / CLOCKS_PER_SEC;
      printf("Processing time: %.8f seconds\n", cpu_time_used);


      merge_and_print_char_counts(segments, num_segments);
      free(buffer);
      for (size_t i = 0; i < num_segments; i++) {
          free(segments[i].char_count);
      }
      free(segments);

      fclose(file);


    // This is to create worker threads or processes and distribute segments
    size_t i;
    for (i = 0; i < num_segments; i++) {
        create_thread_or_process(process_file_segment, &segments[i]);
    }
    wait_for_threads_or_processes();
    // Merge and print the character count results
    merge_and_print_char_counts(segments, num_segments);

    // We need to clean it now and close the file
    free(buffer);
    for (size_t i = 0; i < num_segments; i++) {
        free(segments[i].char_count);
    }
    free(segments);
    fclose(file);
}