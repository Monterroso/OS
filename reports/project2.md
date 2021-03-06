Final Report for Project 2: User Programs
===================================

# Task 1:
The key difference between our design doc and the final solution is we ended up writing the code in the function `setup_stack` instead of `process_execute`.  `setup_stack` provided us with access to the new thread's stack pointer so we could push the arguments.  Otherwise, our implementation matched the doc.

# Task 2:
We hardly strayed from Task 2's doc.  `halt` and `practice` were simply to implement.  For `exec`, we didn't note what to do if the load fails: update the exit status of the thread and kill the thread.  The data structures we designed worked splendidly.

# Task 3:
One of the main issues we ran into was the difficulty of properly printing the correct outputs and returning the correct error value. We originally wanted to implement most of our functionality within `Syscall.c`, but the difficulty arose when we tried to set our return value and our error messages, because what we wanted was for all of that to be handled when the thread exited in the kill process function, and that was not in `Syscall.c`. So we modified process exit in `process.c` in order to fulfil that requirement. Other than that, we did not have a significant issue implementing the file system functionality. We did also use a linked list in order to store all of our file handlers with their appropriate fd values, and we added a structure with a list element and the file handler and fd in order to serve as the elements in that linked list.


# Testing Report

## Test 1: `excess-stack`
This test was designed to test the argument pushing of task 1.  It checks the value of the stack pointer after passing in three arguments to make sure the kernel doesn't bloat the stack with unnecessary padding or data.  Based on the value of `PHYS_BASE`, we can calculate an address below that, allocating space for the arguments, zero padding, address array, argv, argc, and an arbitrary return pointer.

### excess-stack.output
~~~
Copying tests/userprog/excess-stack to scratch partition...
  2 qemu -hda /tmp/70HRWlBEYN.dsk -m 4 -net none -nographic -monitor null
  3 PiLo hda1^M
  4 Loading..........^M
  5 Kernel command line: -q -f extract run 'excess-stack a b c'
  6 Pintos booting with 4,088 kB RAM...
  7 382 pages available in kernel pool.
  8 382 pages available in user pool.
  9 Calibrating timer...  104,755,200 loops/s.
 10 hda: 5,040 sectors (2 MB), model "QM00001", serial "QEMU HARDDISK"
 11 hda1: 167 sectors (83 kB), Pintos OS kernel (20)
 12 hda2: 4,096 sectors (2 MB), Pintos file system (21)
 13 hda3: 100 sectors (50 kB), Pintos scratch (22)
 14 filesys: using hda2
 15 scratch: using hda3
 16 Formatting file system...done.
 17 Boot complete.
 18 Extracting ustar archive from scratch device into file system...
 19 Putting 'excess-stack' into the file system...
 20 Erasing ustar archive...
 21 Executing 'excess-stack a b c':
 22 (excess-stack) Argument base: 0xbfffffb0
 23 excess-stack: exit(0)
 24 Execution of 'excess-stack a b c' complete.
 25 Timer: 54 ticks
 26 Thread: 0 idle ticks, 54 kernel ticks, 0 user ticks
 27 hda2 (filesys): 62 reads, 204 writes
 28 hda3 (scratch): 99 reads, 2 writes
 29 Console: 926 characters output
 30 Keyboard: 0 keys pressed
 31 Exception: 0 page faults
 32 Powering off...
~~~

### excess-stack.result
~~~
PASS
~~~

One kernel bug that would conflict with this test is if the kernel adds its own padding or doesn't store the args at `PHYS_BASE`.  The stack pointer would be offset as a result, making the task 1 code seem flawed.

Another potential kernel bug is the physical memory allocation.  Our test verifies that the thread's args in virtual memory do not pass a certain virtual address.  While this may pass, the kernel may poorly allocate physical memory and waste space while still mapping efficiently to virtual memory.  This would result in false positives; poor memory use would still pass the test.

## Test 2: SeekandTell
This tests seek tell and filesize in Task 3. These were not explicitly tested and we needed to get these functions working before the other tests would work. The test creates a file. We use seek to move to a position and call tell to identify our position. By comparing the position used when calling seek and the value returned by tell, we can identify whether both syscalls work. To test filesize, we write text to the file and the syscall will return amount of bytes it wrote. We compare this value to what is returned by filesize. 

Output
~~~
Copying tests/userprog/seekandtell to scratch partition...
Copying ../../tests/userprog/sample.txt to scratch partition...
qemu -hda /tmp/SPG1M3l4RD.dsk -m 4 -net none -nographic -monitor null
PiLo hda1
Loading..........
Kernel command line: -q -f extract run seekandtell
Pintos booting with 4,088 kB RAM...
382 pages available in kernel pool.
382 pages available in user pool.
Calibrating timer...  242,073,600 loops/s.
hda: 5,040 sectors (2 MB), model "QM00001", serial "QEMU HARDDISK"
hda1: 167 sectors (83 kB), Pintos OS kernel (20)
hda2: 4,096 sectors (2 MB), Pintos file system (21)
hda3: 106 sectors (53 kB), Pintos scratch (22)
filesys: using hda2
scratch: using hda3
Formatting file system...done.
Boot complete.
Extracting ustar archive from scratch device into file system...
Putting 'seekandtell' into the file system...
Putting 'sample.txt' into the file system...
Erasing ustar archive...
Executing 'seekandtell':
(seekandtell) begin
(seekandtell) create "test.txt"
(seekandtell) open "test.txt"
(seekandtell) end
seekandtell: exit(0)
Execution of 'seekandtell' complete.
Timer: 71 ticks
Thread: 7 idle ticks, 62 kernel ticks, 3 user ticks
hda2 (filesys): 116 reads, 224 writes
hda3 (scratch): 105 reads, 2 writes
Console: 1007 characters output
Keyboard: 0 keys pressed
Exception: 0 page faults
Powering off...
~~~

Result: 
~~~
PASS
~~~

Because this test tests the basic functionality of these syscalls, a kernel bug this test would catch would be an incorrect implementation of any of the syscalls. 

## Test 3: NullPoint
This test tries to write to a file with an uninitialized buffer. In our test, we create a file and and try to write to it. If the write syscall passes without crashing, then we fail the test as the expected result is that it should exit trying to perform this operation. 

Output
~~~
Copying tests/userprog/nullpoint to scratch partition...
qemu -hda /tmp/xQmVYDzOqb.dsk -m 4 -net none -nographic -monitor null
PiLo hda1
Loading..........
Kernel command line: -q -f extract run nullpoint
Pintos booting with 4,088 kB RAM...
382 pages available in kernel pool.
382 pages available in user pool.
Calibrating timer...  314,163,200 loops/s.
hda: 5,040 sectors (2 MB), model "QM00001", serial "QEMU HARDDISK"
hda1: 167 sectors (83 kB), Pintos OS kernel (20)
hda2: 4,096 sectors (2 MB), Pintos file system (21)
hda3: 103 sectors (51 kB), Pintos scratch (22)
filesys: using hda2
scratch: using hda3
Formatting file system...done.
Boot complete.
Extracting ustar archive from scratch device into file system...
Putting 'nullpoint' into the file system...
Erasing ustar archive...
Executing 'nullpoint':
(nullpoint) begin
(nullpoint) create "test.txt"
(nullpoint) open "test.txt"
nullpoint: exit(-1)
Execution of 'nullpoint' complete.
Timer: 69 ticks
Thread: 6 idle ticks, 60 kernel ticks, 3 user ticks
hda2 (filesys): 87 reads, 215 writes
hda3 (scratch): 102 reads, 2 writes
Console: 928 characters output
Keyboard: 0 keys pressed
Exception: 0 page faults
Powering off...
~~~

Result:
~~~
PASS
~~~

A kernel bug this test would catch is when we try to call write on a buffer that doesn't exist or when we call write on an uninitialized buffer. In either of those cases, this test will fail. 

## Contributions
**Noah Poole**: Completed Task 1 and 2, designed 1 test.

**Marcus**: Wrote alot of Task 3

**Johnathan**: Contributed to Task 3, mostly debugging. Looked at test cases to see what wasn't tested.

**William**: 2 tests and debugging

Our group did well to not procrastinate on this project.  We had ample time to discuss and revise our document without feeling too pressured.  Still, we should get together another week earlier to have an in-depth discussion of our design document to make sure all members understand and agree.
