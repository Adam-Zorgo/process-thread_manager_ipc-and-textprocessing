#include "../headers/ipctesting.h"
#include <mqueue.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <unistd.h>

#define SHARED_MEM_SIZE 512
#define MSG_QUEUE_NAME "/my_queue"
#define MAX_MSG_SIZE 512

typedef struct {
  char data[SHARED_MEM_SIZE];
  sem_t sem;
} shared_mem_t;

shared_mem_t *create_shared_mem() {
  shared_mem_t *shared_mem =
      mmap(NULL, sizeof(shared_mem_t), PROT_READ | PROT_WRITE,
           MAP_SHARED | MAP_ANONYMOUS, -1, 0);
  if (shared_mem == MAP_FAILED) {
    perror("mmap");
    return NULL;
  }
  sem_init(&shared_mem->sem, 1, 1);
  return shared_mem;
}

void write_to_shared_mem(shared_mem_t *shared_mem, const char *message) {
  sem_wait(&shared_mem->sem);
  strncpy(shared_mem->data, message, SHARED_MEM_SIZE - 1);
  shared_mem->data[SHARED_MEM_SIZE - 1] = '\0';
  sem_post(&shared_mem->sem);
}

char *read_from_shared_mem(shared_mem_t *shared_mem) {
  sem_wait(&shared_mem->sem);
  char *data = strdup(shared_mem->data);
  sem_post(&shared_mem->sem);
  return data;
}

mqd_t create_msg_queue() {
  struct mq_attr attr;
  attr.mq_flags = 0;
  attr.mq_maxmsg = 5;
  attr.mq_msgsize = MAX_MSG_SIZE;
  attr.mq_curmsgs = 0;

  mqd_t mqd = mq_open(MSG_QUEUE_NAME, O_CREAT | O_RDWR, 0644, &attr);
  if (mqd == (mqd_t)-1) {
    perror("mq_open");
    return (mqd_t)-1;
  }

  return mqd;
}

int send_message(mqd_t mqd, const char *message) {
  return mq_send(mqd, message, strlen(message) + 1, 0);
}

char *receive_message(mqd_t mqd) {
  char buffer[MAX_MSG_SIZE];
  ssize_t size = mq_receive(mqd, buffer, MAX_MSG_SIZE, NULL);
  if (size == -1) {
    perror("mq_receive");
    return NULL;
  }
  buffer[size] = '\0';
  return strdup(buffer);
}

void test_shared_mem_ipc() {
  shared_mem_t *shared_mem = create_shared_mem();
  if (shared_mem == NULL) {
    printf("Failed to create shared memory\n");
    return;
  }

  // Write to shared memory
  const char *message = "Hello, shared memory!";
  write_to_shared_mem(shared_mem, message);

  // Read from shared memory
  char *data = read_from_shared_mem(shared_mem);
  printf("Shared memory data: %s\n", data);
  free(data);

  // Clean up shared memory
  sem_destroy(&shared_mem->sem);
  munmap(shared_mem, sizeof(shared_mem_t));
}

void test_msg_queue_ipc() {
  mqd_t mqd = create_msg_queue();
  if (mqd == (mqd_t)-1) {
    printf("Failed to create message queue\n");
    return;
  }

  // Send a message
  const char *message = "Hello, message queue!";
  if (send_message(mqd, message) == -1) {
    printf("Failed to send message\n");
    mq_close(mqd);
    mq_unlink(MSG_QUEUE_NAME);
    return;
  }

  // Receive a message
  char *data = receive_message(mqd);
  if (data == NULL) {
    printf("Failed to receive message\n");
    mq_close(mqd);
    mq_unlink(MSG_QUEUE_NAME);
    return;
  }

  printf("Message queue data: %s\n", data);
  free(data);

  // Clean up message queue
  mq_close(mqd);
  mq_unlink(MSG_QUEUE_NAME);
}
