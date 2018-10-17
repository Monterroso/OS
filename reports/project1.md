Final Report for Project 1: Threads
===================================

# Task 1:
There were two changes we made to our design for Task 1.

- First, we chose to use the thread_block and thread_unblock functions over the semaphore abstraction.  Task 2 required semaphores to unblock based on priority, and timer_sleep needs to unblock based on time remaining, so using a single semaphore for blocking all sleeping threads would be troublesome.
- Second, we did not need to write our own max heap structure or functions.  After reviewing the skeleton code, we found list_max adequately filled our needs; the linear search time had no noticeable impact on our code's performance.

# Task 2:
There were two changes we made to our design for Task 2.  

1. We decided not to give a lock a priority and just calculated from the thread's list
2. In our design doc, we did not have a waiting on lock variable so we had to add it in. 

Aside from that everything was generally the same as described in the design doc.

# Task 3:
There was only one main change we made to our design. We added the method add_thread_to_queue to add a thread that is ready into the mlfqs. During the writing of the design doc, we did not take into account that there would be multiple different points in which we'd add a thread to the queue, thus creating this method would make coding more convenient. 

## Contributions
**Noah Poole**: Completed Task 1, helped code Task 2 and debug Task 3.

**Marcus**: Mainly helped debug Task 2 and plan out test cases and our implimentation.

**Johnathan**: Debugging: Task2 wasnt working and found that it was in task 1(debugging), also debugged test in 2 and in 3

**William**: Implemented Task 3 and helped debug Task 3.


The group did a good job of discussing and debating the merits of various designs.  We all helped each other debug difficult problems.  In retrospect, splitting the work into tasks was not ideal, and we all should have worked together on every task.  This would have helped us understand every element of the project and spot issues earlier on.
