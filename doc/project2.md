Design Document for Project 2: User Programs
============================================

## Group Members

* Noah Poole <_jellyfish_@berkeley.edu>
* Johnathan Chow <johnathan.chow18@berkeley.edu>
* William Ju <w99j99@berkeley.edu>
* Marcus Monterroso <mmonterroso@berkeley.edu>

# Task 1: Argument Passing

## Data Structures and Functions
```
tid_t process_execute (const char *file_name)
Need to modify this function pass arguments to the process
```  
## Algorithms
In `process_execute`, parse the input `file_name`, delimiting with spaces.  After the thread for the file has been created, use `tid` to get a reference to the thread's memory page.  We then push the various arguments to the thread's stack.

## Synchronization
Threads can be scheduled before `process_execute` is finished returning, so we may block the thread before pushing its arguments to the stack.  Otherwise, there may be concurrent memory access errors.  We can then unblock the thread after we finish updating its arguments.

## Rationale
### Advantages
- Simple implementation
- Builds off of existing functions

### Disadvantages
- Blocking the thread may not be necessary and may slow execution of the program

# Task 2
## Data Structures
```
process.h
struct node{
  pid_t pid;
  bool loaded;
	
  struct list_elem children;
  int waiting;
  int exit_status;
  semaphore sema;
}

thread.h
struct thread {
    struct pnode* pnode;
    struct list children;
}
```
## Algorithms

### Exec
Call `process_execute()` on input and get the tid of the process.

In `init_thread()`, init the node and add children to the thread with waiting to 0. When thread finishes loading the program in `start_process()`, it will increase waiting by 1.

In exec syscall, we will call `sema_down()` on the node associated with the child whose pid equals the returned tid. If the node has loaded, we will return the pid of the child otherwise returning error (-1). At the end of exec syscall, call `sema_up()`.

### Halt
Call `shutdown()`.

### Wait
Look for a node in the current thread's children with the correct pid. Call `sema_down()` on that child, pop the child and return the `exit_status`.

Call `sema_up()` on the node's semaphore at the end of `process_exit()`. The exit_status will be whatever the exit syscall returns.

### Synchronization
Should work because we will keep track of the exit_status and will be able to run if child runs and terminates before parent has time to return.

### Rationale
Decides to use semaphores to see if threads and memory are available. Also it would be nice to take advantage of semaphores to track processes. A list of children will allow the syscalls to keep track of the parent child relationships. We needed to check that the which runs first, whether its child or parent first.

# Task 3

## Data Structures and Functions
```
process.c:
process_execute (const char *file_name)
Need to set `file_name` to read only

file.c:
struct file
Add an int fd field to the struct

syscall.c:
struct lock file_lock
Lock held by thread currently using file system

struct file ** file_list
Array of pointers to files currently opened by the thread

syscall_handler (struct intr_frame *f UNUSED)
Add calls to file syscall handlers when prompted

bool create (const char *file, unsigned initial size)
Lock, call filesys_create, and release if no files open

bool remove (const char *file)
Lock, call filesys_remove, and release if no files open

int open (const char *file)
Lock, call filesys_open, and add file to list

int filesize (int fd)
Call file_length

int read (int fd, void *buffer, unsigned size)
Call file_read or use input_getc() to read from stdin

int write (int fd, const void *buffer, unsigned size)
Call file_write or use putbuf() to write to stdout

void seek (int fd, unsigned position)
Call file_seek

unsigned tell (int fd)
Call file_tell

void close (int fd)
Call file_close
```  
## Algorithms
Most of the syscalls can simply call their file.c and filesys.c equivalents.  The functions will use the file descriptor to lookup file structures in `file_list`, then pass those structs to the right functions.  Some functions require more code, notable `open`, `close`, `read`, and `write`.  `open` needs to also add the new file struct to `file_list` and assign it a unique file descriptor while `close` should remove the struct from the list.  `read` needs use `input_getc` if reading from stdin.  `write` needs to use `putbuf` if writing to stdout.

## Synchronization
The main form of synchronization will be a single global lock in the syscall.c file.  Any thread that calls a file syscall will acquire the lock.  As long as a thread has an open file, it will hold the lock.  Thus, `open()` will not release the lock unless the open fails.  When the lock is released at the end of any function, we first check that the current thread doesn't have any open files.

## Rationale
### Advantages
- A single lock lends easy implementation and abstraction
- Calling functions from file.c and filesys.c is hard to mess up
- Adding `fd` to the `struct file` allows for easy tracking of file descriptors

### Disadvantages
- Only one thread can perform any file operations at any time
- Threads cannot even access completely separate files at the same time
- Need to store array of pointers

### Additional Questions


The test sc-bad-sp.c attempts to use a bad stack pointer. On line 18, assembly code is used, along with the volatile keyword to prevent the instruction from being removed. Then, the stack pointer (The $sp register) is replaced (via a movl) with a horrifying value, one that is clearly not acceptable. The Operating system should prevent this from occurring and kill the process.
 
The test sc-boundary-2.c sets up a valid stack pointer on line 20, using the step from described from the previous, however it also attempts to push an array “p” into memory. However, on lines 15, 16, and 17, the array is created using the area of the boundary – 7. This makes it such that the first element is on the previous page, and all the others are on the latter. This causes the sys_call (defined on line 16) to be in an invalid location, meaning the process should exit.  
One such place the tests are missing come from the lack of extensive testing by multithreading, that during the process of a sys call, so while in kernel mode, the thread cannot be interrupted, we do not want sys calls to be interrupted at critical points.  

