#include "userprog/syscall.h"
#include <stdio.h>
#include <syscall-nr.h>
#include "threads/interrupt.h"
#include "threads/thread.h"

static void syscall_handler (struct intr_frame *);

void
syscall_init (void)
{
  intr_register_int (0x30, 3, INTR_ON, syscall_handler, "syscall");
}

static void
syscall_handler (struct intr_frame *f UNUSED)
{
  uint32_t* args = ((uint32_t*) f->esp);

  switch (args[0]) {

    /* TASK 2 SYSCALLS */

    case SYS_EXIT :
      f->eax = args[1];
      printf("%s: exit(%d)\n", &thread_current ()->name, args[1]);
      thread_exit();
      break;

    case SYS_PRACTICE :
      f->eax = args[1] + 1;
      break;

    case SYS_HALT :
      shutdown_power_off();
      break;

    case SYS_EXEC :
      //f->eax = process_execute(args[1]);
      break;

    case SYS_WAIT :
      break;

    /* TASK 3 FILE SYSCALLS */

    case SYS_WRITE :
      if (args[1] == 1) {
        putbuf(args[2], args[3]);
        f->eax = args[3];
      }
      break;

  }
}

int verify_pointer(void * ptr, void * esp) {
  return 1;
}
