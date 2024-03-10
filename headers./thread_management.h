#ifndef THREAD_MANAGEMENT_H
#define THREAD_MANAGEMENT_H

#include <pthread.h>
#include "constants.h"

int create_thread(int process_id);
void suspend_thread(int process_id, pthread_t thread_id);
void resume_thread(int process_id, pthread_t thread_id);
int find_thread(int process_id, pthread_t thread_id);
int kill_thread(int process_id, pthread_t thread_id);

#endif