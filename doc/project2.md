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

### Task 2

### Task 3

### Additional Questions
