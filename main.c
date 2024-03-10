#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include <sys/resource.h>
#include <sys/time.h>
#include "headers/thread_management.h"
#include "headers/process_management.h"
#include "headers/file_processing.h"
#include "headers/constants.h"

void clear_screen() {
#ifdef _WIN32
    system("cls");
#else
    system("clear"); // If it isnt windows
#endif
}

int main() {
  int choice, process_id;
  unsigned long thread_id;
  // test_ipc_performance();
  while (1) {
    printf("\n** Menu **\n");
    printf("1. Create Process\n");
    printf("2. Create Thread\n");
    printf("3. List Processes\n");
    printf("4. Kill Process\n");
    printf("5. Suspend Process\n");
    printf("6. Resume Process\n");
    printf("7. Kill Thread\n");
    printf("8. Suspend Thread\n");
    printf("9. Resume Thread\n");
    printf("10. Process File\n");
    printf("11. Compare IPC Performances\n");
    printf("12. Exit\n");
    printf("Enter your choice: ");
    scanf("%d", &choice);

    switch (choice) {
      case 1:
        clear_screen();
        create_process();
        break;
      case 2:
        clear_screen();
        printf("Enter process ID to create thread: ");
        scanf("%d", &process_id);
        create_thread(process_id);
        break;
      case 3:
        clear_screen();
        list_processes();
        break;
      case 4:
        clear_screen();
        printf("Enter process ID to kill: ");
        scanf("%d", &process_id);
        kill_process(process_id);
        break;
      case 5:
        clear_screen();
        printf("Enter process ID to suspend: ");
        scanf("%d", &process_id);
        suspend_process(process_id);
        break;
      case 6:
        clear_screen();
        printf("Enter process ID to resume: ");
        scanf("%d", &process_id);
        resume_process(process_id);
        break;
      case 7:
        clear_screen();
        printf("Enter process ID and thread ID to kill: ");
        scanf("%d %lu", &process_id, &thread_id);
        kill_thread(process_id, thread_id);
        break;
      case 8:
        clear_screen();
        printf("Enter process ID and thread ID to suspend: ");
        scanf("%d %lu", &process_id, &thread_id);
        suspend_thread(process_id, thread_id);
        break;
      case 9:
        clear_screen();
        printf("Enter process ID and thread ID to resume: ");
        scanf("%d %lu", &process_id, &thread_id);
        resume_thread(process_id, thread_id);
        break;
      case 10:
        clear_screen();
        printf("Enter file name: ");
        char filename[256];
        scanf("%s", filename);
        read_and_distribute_file(filename);
        break;
      case 11:
        clear_screen();
        test_ipc_performance();
        break;
      case 12:
          clear_screen();
          printf("Exiting...\n");
          exit(0);
      default:
          printf("Invalid choice\n");
    }
  }
  wait_for_threads_or_processes();

  return 0;
}