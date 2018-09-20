Design Document for Project 1: Threads
======================================

## Group Members

* Noah Poole <_jellyfish_@berkeley.edu>
* FirstName LastName <email@domain.example>
* FirstName LastName <email@domain.example>
* FirstName LastName <email@domain.example>

# Task 1: Efficient Alarm Clock

## Data Structures and Functions
```
struct semaphore sema;  
sema_init(&sema, 0);  
The above semaphore blocks and unblocks threads that call timer_sleep().  
  
struct list sleep_times;  
list_init(&sleep_times);  
This list holds the remaining time of each sleeping thread.  
  
int floatEntry(struct list * minHeap);  
This function takes in a list and floats the value located at the end of the list.  Then, it returns final index of the value.  
  
void sinkEntry(struct list * minHeap);  
This function replaces the 0th element of the minHeap, and restructures the heap so it follows the proper conventions.  
```  
## Algorithms
#### Inside function `timer_sleep()`
When a thread calls `timer_sleep`(), the number of ticks it will sleep for will be added to `sleep_times`.  Then, `sema` will be downed to block the current thread.  `floatEntry` will be called on `sleep_times` and the returned index will be used to restructure the list contained by `sema` to match `sleep_times`.  
#### Inside function `timer_tick()`
Every time this function is called, every entry of `sleep_times` will be decremented.  If the first entry of `sleep_times` has reached 0, `sema` will be upped and a thread will be unblocked.  `sinkEntry` will be called on `sleep_times` and the list of threads will be rearranged to match the new ordering.

## Synchronization
If multiple threads call timer_sleep, there could be a synch error when adding them to the min heap.  This is easily fixed by adding another semaphore that is downed before the `floatEntry` call and upped at the end of the call.  Only one thread will be added to the heap at any time.  
We also don't have to worry about threads becoming dereferenced while we are putting them to sleep because downing a semaphore turns off interrupts.

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

# Task 3: MLFQS

## Data Structures and Functions
```
Inside the thread struct:
fixed_point_t nice 
fixed_point_t recent_cpu
Both these values used for updating the thread's priority.

fixed_point_t load_avg
fixed_point_t ready_threads
Also used to help calculate a thread's priority.

struct list mlfpqs[64]
Will hold our 64 priority queues.

void thread_set_nice(int nice)
Will be modified to be implemented.

void thread_get_nice(int nice)
Will be modified to be implemented.

void thread_get_load_avg(int nice)
Will be modified to be implemented.

void thread_get_recent_cpu(int nice)
Will be modified to be implemented.

static void init_thread(struct thread *t, const char *name, int priority)
Will be modified to initialize nice and recent_cpu

void thread_init (void)
Will be modified to intialize load_avg and ready_threads

static struct thread *next_thread_to_run (void)
Will be modified to implement MLFQS and update the thread's priority/recent_cpu

void update_mlfqs(void)
Will be used to update load_avg and every thread's recent_cpu and priority.
```

## Algorithms
Our MLFQS will be a list of 64 queues where the index of the list represents the priority level of the queue. For every thread, there will be a recent_cpu and nice value to help calculate its priority. For each tick of the running thread, we increase the value of recent_cpu, so after four ticks, the thread will have a lower priority. In addition, we will have a load_avg and ready_threads to help calculate the priority of threads.

Every second, we will need to update load_avg as well as every thread's recent_cpu and priority using update_mlfpqs. This will be implemented by first recalculating load_avg. Then for each thread, we will update recent_cpu before updating priority. The reason why we do it in this order is because calculating recent_cpu depends on load_avg and we want to use the more updated version of load_avg. Likewise, calculating priority depends on recent_cpu and we want to use the more updated version of recent_cpu.

For the case where there are multiple threads in the highest priority queue, we will use the round robin algorithm where each thread will have 4 ticks to run. We will decide which thread to run within the queue by using first in first out.

## Synchronization
All of our values are updated all together at once using update_mlfqs. Because of this there won't be any synchronization issues. On top of this, the value of the thread that is currently running will only be updated one at a time. Two threads won't have their recent_cpu or priority values changed at the same time.

## Rationale
There were many ways to decide on how to decide which thread to run when there are multiple threads in the highest priority queue. One of the most logical ways was to pick the thread that had the lowest niceness value. While this method makes sense, it involves checking through every thread's niceness and will take linear time to decide which thread to run. By using a queue and using first in first out, it will take us constant time to choose which thread we will run in this case, which is a lot faster than using niceness. 
