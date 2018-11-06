Final Report for Project 2: User Programs
===================================

# Task 1:
The key difference between our design doc and the final solution is we ended up writing the code in the function setup_stack instead of process_execute.  setup_stack provided us with access to the new thread's stack pointer so we could push the arguments.  Otherwise, our implementation matched the doc.

# Task 2:
We hardly strayed from Task 2's doc.  Halt and practice were simply to implement.  For exec, we didn't note what to do if the load fails: update the exit status of the thread and kill the thread.  The data structures we designed worked splendidly.

# Task 3:


## Contributions
**Noah Poole**: Completed Task 1 and 2, designed 1 test.

**Marcus**: 

**Johnathan**: 

**William**: 


Our group did well to not procrastinate on this project.  We had ample time to discuss and revise our document without feeling too pressured.  Still, we should get together another week earlier to have an in-depth discussion of our design document to make sure all members understand and agree.

~~~~~~~~~~~~~~~~~~~~~~~~~

#Testing Report

##Test 1: excess-stack
This test was designed to test the argument pushing of task 1.  It checks the value of the stack pointer after passing in three arguments to make sure the kernel doesn't bloat the stack with unnecessary padding or data.  Based on the value of PHYS_BASE, we can calculate an address below that, allocating space for the arguments, zero padding, address array, argv, argc, and an arbitrary return pointer.

One kernel bug that would conflict with this test is if the kernel adds its own padding or doesn't store the args at PHYS_BASE.  The stack pointer would be offset as a result, making the task 1 code seem flawed.  Another potential kernel bug is 
