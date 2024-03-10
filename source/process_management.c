#include <stdio.h>
#include <stdlib.h>
#include "../headers/process_management.h"
#include "../headers/thread_management.h"
#include "../headers/constants.h"

#define MAX_PROCESSES 10
#define MAX_THREADS 5

process_control_block_t processes[MAX_PROCESSES];
int num_processes = 0;
int next_thread_index = 0;

int find_process(int process_id) {
  for (int i = 0; i < num_processes; i++) {
    if (processes[i].id == process_id) {
      return i;
    }
  }
  return -1;
}

void create_process() {
  if (num_processes == MAX_PROCESSES) {
    printf("Maximum number of processes reached\n");
    return;
  }
  processes[num_processes].id = num_processes + 1;
  processes[num_processes].state = READY;
  processes[num_processes].num_threads = 0;
  
  pthread_mutex_init(&processes[num_processes].mutex, NULL);
  num_processes++;
  printf("Process %d created\n", processes[num_processes - 1].id);
}

void suspend_process(int process_id) {
  int process_index = find_process(process_id);
  if (process_index == -1) {
    printf("Process %d not found\n", process_id);
    return;
  }
  pthread_mutex_lock(&processes[process_index].mutex);
  if (processes[process_index].state == RUNNING) {
    processes[process_index].state = WAITING;
    printf("Process %d suspended\n", process_id);
  } else {
    printf("Process %d is not running\n", process_id);
  }
  pthread_mutex_unlock(&processes[process_index].mutex);
}

void resume_process(int process_id) {
  int process_index = find_process(process_id);
  if (process_index == -1) {
    printf("Process %d not found\n", process_id);
    return;
  }
  pthread_mutex_lock(&processes[process_id].mutex);
  if (processes[process_index].state == WAITING) {
    processes[process_index].state = READY;
    printf("Process %d resumed\n", process_id);
  } else {
    printf("Process %d is not suspended\n", process_id);
  }
  pthread_mutex_unlock(&processes[process_index].mutex);
}

void kill_process(int process_id) {
  int process_index = find_process(process_id);
  if (process_index == -1) {
    printf("Process %d not found\n", process_id);
    return;
  }

  pthread_mutex_lock(&processes[process_index].mutex);
  processes[process_index].state = TERMINATED;
  pthread_mutex_unlock(&processes[process_index].mutex);

  printf("Process %d killed\n", process_id);
}

void list_processes() {
  printf("\n** Process List **\n");
  printf("PID\tState\tThreads\n");
  for (int i = 0; i < num_processes; i++) {
    printf("%d\t%d\t%d\n", processes[i].id, processes[i].state, processes[i].num_threads);
    for (int j = 0; j < processes[i].num_threads; j++) {
      printf("\t- Thread ID: %lu (LWP State: %s)\n", processes[i].thread_ids[j], processes[i].lwp_states[j] == LWP_IDLE ? "Idle" : "Running");
    }
  }
}