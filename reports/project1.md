Final Report for Project 1: Threads
===================================

# Task 1:
There were two changes we made to our design for Task 1.
1.
First, we chose to use the thread_block and thread_unblock functions over the semaphore abstraction.  Task 2 required semaphores to unblock based on priority, and timer_sleep needs to unblock based on time remaining, so using a single semaphore for blocking all sleeping threads would be troublesome.
2.
Second, we did not need to write our own max heap structure or functions.  After reviewing the skeleton code, we found list_max adequately filled our needs; the linear search time had no noticeable impact on our code's performance.

# Task 2:

# Task 3:

## Contributions
Noah Poole: Completed Task 1, helped code Task 2 and debug Task 3.
Marcus:
Johnathan:
William:

The group did a good job of discussing and debating the merits of various designs.  We all helped each other debug difficult problems.  In retrospect, splitting the work into tasks was not ideal, and we all should have worked together on every task.  This would have helped us understand every element of the project and spot issues earlier on.
