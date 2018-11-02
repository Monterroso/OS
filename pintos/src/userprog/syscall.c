#include "userprog/syscall.h"
#include <stdio.h>
#include <syscall-nr.h>
#include "threads/interrupt.h"
#include "threads/thread.h"
#include "threads/vaddr.h"

static void syscall_handler (struct intr_frame *);
//void verify_pointer(void * ptr, struct intr_frame * f);

struct lock file_lock;

void
syscall_init (void)
{
  lock_init(&file_lock);
  intr_register_int (0x30, 3, INTR_ON, syscall_handler, "syscall");
}

static void
syscall_handler (struct intr_frame *f UNUSED)
{
  uint32_t* args = ((uint32_t*) f->esp);
  verify_pointer(args, f);
  verify_pointer(args + 1, f);

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
      f->eax = process_wait (args[1]);
      break;

    /* TASK 3 FILE SYSCALLS */
    //write
    //create
    //remove
    //open
  	// read
  	// seek
  	// tell
  	// close
    case SYS_WRITE :
      verify_pointer(args + 2, f);
      verify_pointer(args + 3, f);
      if (args[1] == 1) {
        putbuf(args[2], args[3]);
        f->eax = args[3];
      } else {
      	//get files with corresponding fd 
      	//call filewrite
      	// if (file_isdir());
      }
      // break;

    // case SYS_CREATE :
    // 	verify_pointer(args + 2, f);
	   //  verify_pointer(args + 3, f);
    // 	f->eax = filesys_create ((char *) args[1], args[2], false);
    
  }
}






struct file *getfile(int fd) {
	// struct list *temp = &thread_current()->file_list;
	// // struct list_elem tempelem = list_begin(temp);
	// for (struct list_elem tempelem = list_begin(temp); tempelem != list_end(temp); tempelem = list_next(temp)){
	// 	struct file tempfile = list_entry(tempelem, struct file, elem);
	// 	if (tempfile->fd == fd) {
	// 		return tempfile;
	// 	}
	// }
	return NULL;

}

int addfile(struct file *in) {
	// struct thread *currythread = thread_current();
	// in->fd = currythread->current_fd++;
	// list_push_back(&currythread->file_list, in);
	// return in->fd;
	return NULL;

}


void verify_pointer(void * ptr, struct intr_frame * f) {
  struct thread * cur = thread_current();
  if (!is_user_vaddr(ptr) || pagedir_get_page(cur->pagedir, ptr) == NULL) {
    cur->info->exit_status = -1;
    f->eax = -1;
    printf("%s: exit(%d)\n", cur->name, -1);
    thread_exit();
  }
}
