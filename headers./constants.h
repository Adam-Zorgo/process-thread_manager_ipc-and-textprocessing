#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <pthread.h>

#define MAX_PROCESSES 10
#define MAX_THREADS 5

#define MAX_FILE_SIZE (1024 * 1024 * 100) // 100 MB
#define SEGMENT_SIZE (1024 * 1024)        // 1 MB

typedef enum { READY, RUNNING, WAITING, TERMINATED } process_state_t;
typedef enum { LWP_IDLE, LWP_RUNNING } lwp_state_t;

typedef struct {
  int id;
  process_state_t state;
  int num_threads;
  pthread_t thread_ids[MAX_THREADS];
  int thread_indices[MAX_THREADS];
  lwp_state_t lwp_states[MAX_THREADS];
  pthread_mutex_t mutex;
} process_control_block_t;

typedef struct {
  char *data;
  size_t size;
  int *char_count;
} file_segment_t;

extern process_control_block_t processes[MAX_PROCESSES];
extern int num_processes;
extern int next_thread_index;

#endif