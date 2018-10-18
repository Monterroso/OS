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

### Task 3

### Additional Questions
