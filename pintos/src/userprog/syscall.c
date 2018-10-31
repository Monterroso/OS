#include "userprog/syscall.h"
#include <stdio.h>
#include <syscall-nr.h>
#include "threads/interrupt.h"
#include "threads/thread.h"
#include "threads/synch.h" 
#include "filesys/file.c"
#include <string.h> 



static void syscall_handler (struct intr_frame *);

//struct semaphore *lock = malloc(sizeof(semaphore));// = sema_init(1);

//sema_init(lock, 1);

void
syscall_init (void)
{
  intr_register_int (0x30, 3, INTR_ON, syscall_handler, "syscall");
}

static void
syscall_handler (struct intr_frame *f UNUSED)
{

  //modify the eax register in order to get a return value. 

  uint32_t* args = ((uint32_t*) f->esp);
  printf("System call number: %d\n", args[0]);

  //we get the length of our elements, to see how many we need
  //int argc = args[2];

  //This points to the arguments right after arc, or arvg
  uint32_t* argv = args + (sizeof(uint32_t) * 3);



  if (args[0] == SYS_EXIT) {
    f->eax = args[1];
    printf("%s: exit(%d)\n", &thread_current ()->name, args[1]);
    thread_exit();
  }

  //(int fd, const void *buffer, unsigned size) 
  //fd is the id of the file stream
  //buffer is the location we write from
  //size is the number of bytes we write from buffer to the file of fd. 
  if (args[0] == SYS_WRITE) {	

  	//sema_down(lock);
  	//so here we need to send in our arguments
  	//want to check if we can use write, or if we should do something else. 

  	//we should check if we are sending to stdout, and it isn't too long
  	if (argv[0] == 1) {

  		//if we write to stdout and we don't write too much, we just write it all in one bout.
  		putbuf((const char*)argv[1], (size_t)argv[2]); 
  		f->eax = strlen((const char*)argv[1]);
  	}/*	
  	else {

  		//if we are not writing to stdout, we can just write to the file normally. 
  		f->eax = write(argv[0], argv[1], argv[2]);
  	}*/
  	
  	//now we release our hold. 
  	//sema_up(lock);

  	//what we should do here, we have access to the stack, which we use to get our arguments
  	//we need three arguments here. 

  }
}
