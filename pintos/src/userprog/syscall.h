#ifndef USERPROG_SYSCALL_H
#define USERPROG_SYSCALL_H

#include "userprog/syscall.h"
#include <stdio.h>
#include <syscall-nr.h>
#include "threads/interrupt.h"
#include "threads/thread.h"
#include "threads/vaddr.h"
#include "filesys/file.h"

#include "filesys/off_t.h"

void syscall_init (void);

void verify_pointer(void * ptr, struct intr_frame *f, bool haslock);

int addfile(struct file *in);

struct file *getfile(int fd);

#endif /* userprog/syscall.h */
