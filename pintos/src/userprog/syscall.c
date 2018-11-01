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
      thread_current()->info->exit_status = args[1];
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
      f->eax = process_execute(args[1]);

      if (f->eax == TID_ERROR)
        break;

      struct list_elem * next;
      struct list * children = &(thread_current()->children);
      struct process_info * info = NULL;
      for (next = list_begin(children); next != list_end(children); next = list_next(next)) {
        info = list_entry(next, struct process_info, elem);
        if (info->pid == f->eax)
          break;
        info = NULL;
      }
      if (info != NULL) {
        sema_down(&(info->load_sema));
        if (!info->loaded) {
	  enum intr_level old = intr_disable();
          list_remove(&(info->elem));
          intr_set_level(old);
	  free(info);
	}
      }
      break;

    case SYS_WAIT :
      f->eax = process_wait(args[1]);
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
