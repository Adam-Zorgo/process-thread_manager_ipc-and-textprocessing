#include "../headers/thread_management.h"
#include "../headers/process_management.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../headers/constants.h"

extern process_control_block_t processes[MAX_PROCESSES];
extern int num_processes;
extern int next_thread_index;

void *generic_thread_function(void *arg) {
  // Simulating thread working
  sleep(1);
  printf("Thread is running\n");
  return NULL;
}

int find_thread(int process_id, pthread_t thread_id) {
  int process_index = find_process(process_id);
  if (process_index == -1) {
    return -1;
  }
  for (int i = 0; i < processes[process_index].num_threads; i++) {
    if (processes[process_index].thread_ids[i] == thread_id) {
      return i;
    }
  }
  return -1;
}

int create_thread(int process_id) {
  int process_index = find_process(process_id);
  if (process_index == -1) {
    printf("Process %d not found\n", process_id);
    return -1;
  }
  if (processes[process_index].num_threads == MAX_THREADS) {
    printf("Maximum number of threads reached in process %d\n", process_id);
    return -1;
  }

  pthread_t thread_id;
  int thread_ret =
      pthread_create(&thread_id, NULL, generic_thread_function, NULL);
  if (thread_ret != 0) {
    printf("Error creating thread: %s\n", strerror(thread_ret));
    return -1;
  }
  // We have to change process data after we create a thread
  processes[process_index]
      .thread_indices[processes[process_index].num_threads] =
      next_thread_index++;
  processes[process_index].thread_ids[processes[process_index].num_threads] =
      thread_id;
  processes[process_index].num_threads++;

  thread_ret = pthread_join(thread_id, NULL); // Waiting for thread to finish
  if (thread_ret != 0) {
    printf("Error joining thread: %s\n", strerror(thread_ret));
    return -1;
  }
  printf("Thread created and joined in process %d with ID %lu\n", process_id,
         thread_id);
  return 0;
}

void suspend_thread(int process_id, pthread_t thread_id) {
  int process_index = find_process(process_id);
  if (process_index == -1) {
    printf("Process %d not found\n", process_id);
    return;
  }
  int thread_index = find_thread(process_id, thread_id);
  if (thread_index == -1) {
    printf("Thread with ID %lu not found in process %d\n", thread_id,
           process_id);
    return;
  }
  pthread_mutex_lock(&processes[process_index].mutex);
  if (processes[process_index].state == RUNNING &&
      processes[process_index].thread_ids[thread_index] == thread_id) {
    processes[process_index].lwp_states[thread_index] =
        LWP_IDLE; // Simulating thread suspention (LWP)
    printf("Thread with ID %lu in process %d suspended\n", thread_id,
           process_id);
  } else {
    printf("Thread with ID %lu cannot be suspended\n", thread_id);
  }
  pthread_mutex_unlock(&processes[process_index].mutex);
}

void resume_thread(int process_id, pthread_t thread_id) {
  int process_index = find_process(process_id);
  if (process_index == -1) {
    printf("Process %d not found\n", process_id);
    return;
  }
  int thread_index = find_thread(process_id, thread_id);
  if (thread_index == -1) {
    printf("Thread with ID %lu not found in process %d\n", thread_id,
           process_id);
    return;
  }

  pthread_mutex_lock(&processes[process_index].mutex);
  if (processes[process_index].state == READY &&
      processes[process_index].thread_ids[thread_index] == thread_id &&
      processes[process_index].lwp_states[thread_index] == LWP_IDLE) {
    processes[process_index].lwp_states[thread_index] =
        LWP_RUNNING; // Simulating the tread being resumed (LWP)
    printf("Thread with ID %lu in process %d resumed\n", thread_id, process_id);
  } else {
    printf("Thread with ID %lu cannot be resumed\n", thread_id);
  }
}

int kill_thread(int process_id, pthread_t thread_id) {
  int process_index = find_process(process_id);
  if (process_index == -1) {
    printf("Process %d not found\n", process_id);
    return -1;
  }
  pthread_mutex_lock(&processes[process_index].mutex);
  int thread_index = find_thread(process_id, thread_id);
  if (thread_index != -1) {
    if (processes[process_index].state !=
        TERMINATED) { // We have to check to make sure we didn't already
                      // terminate it
      int cancel_ret = pthread_cancel(thread_id);
      if (cancel_ret != 0) {
        printf("Error canceling thread: %s\n", strerror(cancel_ret));
      } else {
        // Removing it from the data
        for (int j = thread_index; j < processes[process_index].num_threads - 1;
             j++) {
          processes[process_index].thread_ids[j] =
              processes[process_index].thread_ids[j + 1];
          processes[process_index].thread_indices[j] =
              processes[process_index].thread_indices[j + 1];
          processes[process_index].lwp_states[j] =
              processes[process_index].lwp_states[j + 1];
        }
        processes[process_index].num_threads--;
        printf("Thread with ID %lu in process %d canceled\n", thread_id,
               process_id);
      }
    } else {
      printf("Thread cannot be canceled because the process %d is already "
             "terminated\n",
             process_id);
    }
  } else {
    printf("Thread with ID %lu not found in process %d\n", thread_id,
           process_id);
  }
  pthread_mutex_unlock(&processes[process_index].mutex);

  return 0;
}