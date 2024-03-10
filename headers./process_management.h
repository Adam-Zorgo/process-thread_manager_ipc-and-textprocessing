#ifndef PROCESS_MANAGEMENT_H
#define PROCESS_MANAGEMENT_H

#include <pthread.h>
#include "constants.h"


void create_process();
int find_process(int process_id);
void kill_process(int process_id);
void suspend_process(int process_id);
void resume_process(int process_id);
void list_processes();

#endif