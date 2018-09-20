Design Document for Project 1: Threads
======================================

## Group Members

* Noah Poole <_jellyfish_@berkeley.edu>
* FirstName LastName <email@domain.example>
* FirstName LastName <email@domain.example>
* FirstName LastName <email@domain.example>

# Task 1: Efficient Alarm Clock

## Data Structures and Functions
struct semaphore sema; 
sema_init(&sema, 0); 
The above semaphore blocks and unblocks threads that call timer_sleep().

struct list `sleep_times`; 
list_init(&`sleep_times`); 
This list holds the remaining time of each sleeping thread.

int `floatEntry`(struct list * minHeap); 
This function takes in a list and floats the value located at the end of the list.  Then, it returns final index of the value.

void `sinkEntry`(struct list * minHeap); 
This function replaces the 0th element of the minHeap, and restructures the heap so it follows the proper conventions.

## Algorithms
#### Inside function `timer_sleep()`
When a thread calls `timer_sleep`(), the number of ticks it will sleep for will be added to `sleep_times`.  Then, `sema` will be downed to block the current thread.  `floatEntry` will be called on `sleep_times` and the returned index will be used to restructure the list contained by `sema` to match `sleep_times`.  
#### Inside function `timer_tick()`
Every time this function is called, every entry of `sleep_times` will be decremented.  If the first entry of `sleep_times` has reached 0, `sema` will be upped and a thread will be unblocked.  `sinkEntry` will be called on `sleep_times` and the list of threads will be rearranged to match the new ordering.

## Synchronization

## Rationale
### Advantages
- Semaphores provide easy abstraction and implementation of thread blocking.
- MinHeap structure provides time efficient access to the next thread to wake up.
- Only requires 2 new functions and editing 2 old functions.

### Disadvantages
- Keeping MinHeaps properly ordered requires extra code.
- Syncing `sleep_times` and the thread list in `sema` is confusing.
- `timer_tick` has to decrement an entire list every call.
- When a thread is awaken, `timer_tick` may slow due to a call to `sinkEntry`.
