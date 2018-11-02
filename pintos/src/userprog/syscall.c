#include "userprog/syscall.h"
#include <stdio.h>
#include <syscall-nr.h>
#include "threads/interrupt.h"
#include "threads/thread.h"
#include "threads/vaddr.h"

#include "filesys/off_t.h"
#include "filesys/file.h"

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
      verify_pointer((void*)(args + 2), f);
      verify_pointer((void*)(args + 3), f);
      if (args[1] == 1) {
        putbuf(args[2], args[3]);
        f->eax = args[3];
      //The case we have a valid ID
      } else if (args[1] > 1){


      	//get files with corresponding fd
        //do this by calling thread_get_file(int fd)
        struct file *fi = thread_get_file(int fd);

        //if file is null, lets set the code to -1;
        if (fi == NULL) {
          f->eax = -1;
          return;
        }

        //lets make sure we aren't writing to a directory
        //We can check this another time. 
        /*
        if (file_isdir()) {
          return 0; //lets not write to anything if this is a directory
        }
        */

      	//call filewrite to write to the file, and return it's value
        //filewrite should have all of the functionality we want
        f->eax = file_write (fi, args[2], args[3]);

      	
      }
      //this is in case we are given a value of 0 or lower
      else {
        printf("%s", "You tried accessing a file ID that's 0 or lower");
      }

      break;

    case SYS_CREATE :
    	verify_pointer(args + 2, f);

      //filesys_create should have the functionality we want
    	f->eax = filesys_create ((char *) args[1], args[2]);
      break;

    case SYS_REMOVE :
      //We don't need to clear the fd value
      //once the file is removed, it should 
      //still be able to be accessed by this
      //this thread by the file descriptors

      //This removes the file, obliterating it with lazers
      //pew pew bwwoooooooooooooshshshshs
      f->eax = filesys_remove (args[1]);
      break;

    case SYS_OPEN :
      //we first get the file structure, and open at the same time
      //using the built in filesys_open function
      struct file *fi = filesys_open (args[1]);

      //if it's null, we set the status to -1 for not success
      if (fi == NULL) {
        f->eax = -1;
        return;
      }

      //otherwise we add the file to our list and return the fd
      f->eax = addfile(fi);

      break;

    
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

/*Given a file structure, we return an fd and add it to our list*/
int addfile(struct file *in) {

  //get the current thread
	struct thread *currythread = thread_current();

  //lets get the new value that we want. 
  int new_id = currythread->current_fd++;

  //now we want to create the file_map that will store that info
  struct file_map *fm = create_file_map(in, new_id);

  //now we push the thread onto our thread list and let it sail
  list_push_back(&currythread->file_list, fm);

	return new_id;

}


void verify_pointer(void * ptr, struct intr_frame *f) {
  struct thread * cur = thread_current();
  if (!is_user_vaddr(ptr) || pagedir_get_page(cur->pagedir, ptr) == NULL) {
    cur->info->exit_status = -1;
    f->eax = -1;
    printf("%s: exit(%d)\n", cur->name, -1);
    thread_exit();
  }
}
