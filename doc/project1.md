Design Document for Project 1: Threads
======================================

## Group Members

* Noah Poole <_jellyfish_@berkeley.edu>
* Johnathan Chow <johnathan.chow18@berkeley.edu>
* William Ju <w99j99@berkeley.edu>
* Marcus Monterroso <mmonterroso@berkeley.edu>

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

# Task 2: Priority Scheduler 

## Data Structures and Functions
```
In thread.c
Use these to hold values
int eff_prio;
int base_prio;
struct list locklist

change 
init_thread(thread, name, priority)
next_thread_to_run(void)
thread_get_priority(void)
thread_set_priority(priority)

In synch.c
struct list locklist
int priority
sema_up(semaphore);
lock_init(lock);
lock_acquire(lock);
lock_try_acquire(lock);
lock_release(lock);

for conditional variable
int priority 

```
### Next thread to run and unblock
We will change `next_thread_to_run()` to use a max queue instead of `list_pop_front()`. Popping the front would not be so bad if everything was sorted when it went in and things changed but this way no sorting is needed. In `list.c`, I believe that the function `list_max()` will give us what we need to get the highest priority thread. Although this is not a real max queue, it functions the same as in we "pop" the thread off the "queue" by just straight up removing it. To unblock a thread, we will use the same exact method by just finding the highest priority thread and unblocking it in `sema_up()`.  

### Priorities and locks 
The locks will represent the priority of the thread so that it is easy to tell what the priority is by looking at the locks.
The only time that priority changes is when a thread waits for a lock and the lock's priority is less than the thread. Must be changed in `lock_acquire()`.

### Conditional variables
For the conditional variables, we will just have an extra variable to hold the priority and when we need to handle it, we use a `list_max()` like the others to find the highest priority one. Change in `cond_wait()` and `cond_signal()` to go for the max priority cond.

### Acquire a lock
There isnt much to do here except add the lock to the locklist with its priority.

### Releasing
In `lock_release()`, we will use `list_max()` to find the priority of the waiters and set the priority to that. The thread will then set its priority to the lock's priority afterward.

### Compute effective priority
To compute this, we get the thread's priority with `thread_set_priority()` and then we would just add all the locks' priorities together to get the actual effective prioirty. Basically sum up every priority in a thread.

### Changing threads priority
This should happen only if a lock has been released. We will call `thread_set_priority()` and we set it to the max of the threads priority or the locks priority. This could raise the priority of the locks based on previous conditions.

## Synchronization
Scheduling priorities should be safe since these are run in `sema_up()` and `next_thread_to_run()`. For the donations, there are actually alot of possibilities of data being changed because of interrupts and stuff. Instead of accounting for each and every case, we decided to just disable interrupts during the process so that it can run without the danger of being changed. 

## Rationale 

### Priorities
we considered using a different data structures such as a linked list or a heap or manually sorting a list but when reading through the code, we found `list_max()`. We assume this works so we are going to use it. All this means is we need higher priority to have a higher value. We decided to use this because it makes it simpler because writing or using linked lists could leave lots of bugs with sorting. The sorting must also be done whenever a priority changes and for the correct thing to pop out of the queue. There is no need to sort the whole thing either as long as we know what the max priority is either because it is prone to change and ultimately meaningless as we only need to know the next.

### Waiting list
For the semaphores, we do the same thing. The problem is that semaphores also have their priority updated all the time so sorting is honestly a really big pain. We found that for threading and stuff, it's just easier to find the biggest one and use `list_max()` because thats all the program needs to know since it needs to know what happens next and nothing else. Also we do the same for conditional variables because we only need to know the next thing to do, the highest priority.

### Donation and lock priority

When a lock is released, it may lose the donation property. Then it will move on to the next recipient of the donation which means we will need to find the max priority of threads waiting on the just released thread. There is no need to find the priorities of the wait list of locks when we can just find the max lock. The lock will represent the priority so there is no need to recompute each time.


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
  
# Additional Questions
1.
- Initialize a semaphore and a lock.
- Spawn three threads with priorties A, B, and C where A is highest priority and C is lowest priority.
- Have C acquire the lock.
- Then have A attempt to acquire the lock.
- B and C both down the semaphore.
- Up the semaphore.
- C should acquire the semaphore and runs its print statement "Finished thread C"
If B acquires the semaphore instead of C, "Finished thread B" will print instead, and we know C's priority wasn't raised to A's.

2.
timer ticks | R(A) | R(B) | R(C) | P(A) | P(B) | P(C) | thread to run
------------|------|------|------|------|------|------|--------------
 0          |   0  |   0  |  0   |   63 |   61 |   59 |A
 4          |   4  |   0  |  0   |   62 |   61 |  59  |A
 8          |   8  |   0  |   0  |    61|   61 |  59  |B
12          |   8  |   4  |   0  |   61 |   60 |  59  |A
16          |   12 |   4  |   0  |   60 |  60  |  59  |B
20          |   12 |   8  |   0  |   60 |  59  |  59  |A
24          |   16 |   8  |  0   |   59 |  59  |  59  |C
28          |   16 |   8  |  4   |   59 |  59  |  58  |B
32          |   16 |  12  |  4   |   59 |  58  |  58  |A
36          |   20 |  12  |  4   |   58 |  58  |   58 |C
3. Yes. We had to decide which thread to choose when there was more than 1 thread that had the same priority. At timer ticks 8, thread A and B both had a priority of 61. We used first in first out to decide which thread would run first. In this case, thread B was the first thread to have a priority of 61, thus we chose thread B over thread A. 
