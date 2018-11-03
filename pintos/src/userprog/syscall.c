#include "userprog/syscall.h"
#include <stdio.h>
#include <syscall-nr.h>
#include "threads/interrupt.h"
#include "threads/thread.h"
#include "threads/vaddr.h"

#include "filesys/off_t.h"
#include "filesys/file.h"
#include "filesys/filesys.h"

#include "devices/input.h"

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
      thread_exit();
      break;

    case SYS_PRACTICE :
      f->eax = args[1] + 1;
      break;

    case SYS_HALT :
      shutdown_power_off();
      break;

    case SYS_EXEC :

      // Lets be sure we check we don't have a bad pointer
      verify_pointer((void*)(args[1]), f);

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
    //filesize
  	// read
  	// seek
  	// tell
  	// close
    case SYS_WRITE :

      //lets acquire the lock
      lock_acquire (&file_lock);

      //lets check to be sure the buffer starts in user memory
      verify_pointer((void*)(args[2]), f);

      //lets check to be sure the buffer ends in user memeory
      verify_pointer((void*)(args[2] + args[3]), f);



      //verify_pointer((void*)(args + 3), f);
      if (args[1] == 1) {
        putbuf(args[2], args[3]);
        f->eax = args[3];
      } else if (args[1] == 0) {
        f->eax = -1;

        //lets make sure to release if we are returning 
        lock_release (&file_lock);
        return;        
      //The case we have a valid ID
      } else if (args[1] > 1){


      	//get files with corresponding fd
        //do this by calling thread_get_file(int fd)
        //first argument is the file fd
        struct file *fi = thread_get_file(args[1]);

        //if file is null, lets set the code to -1;
        if (fi == NULL) {
          f->eax = -1;

          //release if we return
          lock_release (&file_lock);
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

      //now at the end, we release the lock
      lock_release (&file_lock);
      break;

    case SYS_CREATE :
    	verify_pointer(args[1], f);

      verify_pointer(args[1] + args[2], f);

      //filesys_create should have the functionality we want
    	f->eax = filesys_create ((char *) args[1], args[2]);
      break;

    case SYS_REMOVE :
      //We don't need to clear the fd value
      //once the file is removed, it should
      //still be able to be accessed by this
      //this thread by the file descriptors
      f->eax = filesys_remove (args[1]);
      break;

    case SYS_OPEN : {
      //we first get the file structure, and open at the same time
      //using the built in filesys_open function
      //lets verify that we aren't given a bad pointer
      verify_pointer(args[1], f);

      //lets just return if we are given a NULL name
      if (args[1] == NULL) {
        f->eax = -1;
        return;
      }

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

    case SYS_FILESIZE : {

      struct file *fi = thread_get_file(args[1]);

      if (fi == NULL) {
        f->eax = -1;
        return;
      }

      f->eax = file_length (fi);
      break;
    }

    case SYS_READ : {

      //lets get our lock now
      lock_acquire (&file_lock);

      //lets check to be sure the buffer starts in user memory
      verify_pointer((void*)(args[2]), f);

      //lets check to be sure the buffer ends in user memeory
      verify_pointer((void*)(args[2] + args[3]), f);

      //lets check if we are reading from 0 or something else
      if (args[1] == 0) {
        f->eax = input_getc();
        lock_release (&file_lock);
        return;
      }

      //check valid for buffer, and that it doesn't write more than it can?

      //otherwise we want to read from the file

      //We get the file struct of the file given the fd
      struct file *fi = thread_get_file(args[1]);

      if (fi == NULL) {
        f->eax = -1;
        lock_release (&file_lock);
        return;
      }


      //now we read
      //file_read (struct file *file, void *buffer, off_t size)
      f->eax = file_read (fi, args[2], args[3]);

      lock_release (&file_lock);

      break;

    }

    case SYS_SEEK : {
      //do we need to do anything else for this?

      //We just want get the file from the fd.
      struct file *fi = thread_get_file(args[1]);

      //now we just call seek
      file_seek (fi, args[2]);

      break;
    }

    case SYS_TELL : {

      struct file *fi = thread_get_file(args[1]);

      //it's possible we need to incriment this by 1.
      f->eax = file_tell(fi);

      break;
    }

    case SYS_CLOSE : {
      //this gives us the file from the file description.
      struct file_map *filmp = thread_get_file_struct(args[1]);

      //lets check if we got something
      if (filmp == NULL) {
        return;
      }

      //we close it, giving the
      file_close (filmp->fi);

      //now we remove it from our list;
      list_remove(&(filmp->elem));

      break;
    }

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
  list_push_back(&currythread->file_list, &fm->elem);

	return new_id;

}


void verify_pointer(void * ptr, struct intr_frame *f) {
  struct thread * cur = thread_current();
  if (!is_user_vaddr(ptr) || pagedir_get_page(cur->pagedir, ptr) == NULL) {
    cur->info->exit_status = -1;
    f->eax = -1;
    thread_exit();
  }
}
