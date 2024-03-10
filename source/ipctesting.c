#include <stdio.h>
#include <time.h>
#include "../headers/timer.h"

// Declare the IPC testing functions as extern
extern void test_shared_mem_ipc();
extern void test_msg_queue_ipc();

void test_ipc_performance() {
    double shared_mem_time = get_execution_time(test_shared_mem_ipc);
    printf("Shared memory IPC execution time: %f seconds\n", shared_mem_time);

    double msg_queue_time = get_execution_time(test_msg_queue_ipc);
    printf("Message queue IPC execution time: %f seconds\n", msg_queue_time);
}