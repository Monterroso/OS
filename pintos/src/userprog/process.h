#ifndef USERPROG_PROCESS_H
#define USERPROG_PROCESS_H

#include "threads/thread.h"

tid_t process_execute (char *file_name);
int process_wait (tid_t);
void process_exit (void);
void process_activate (void);

struct process_info {
  pid_t pid;
  bool loaded;
  struct list_elem elem;
  int waiting;
  int exit_status;
  struct semaphore wait_sema;
  struct semaphore load_sema;
}

#endif /* userprog/process.h */
