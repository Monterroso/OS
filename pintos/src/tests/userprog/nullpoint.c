/* Tests that writing a nul pointer as buffer will crash,
based off of write-normal */

#include <syscall.h>
#include "tests/userprog/sample.inc"
#include "tests/lib.h"
#include "tests/main.h"

void
test_main (void)
{
  int handle, size, temp;
  char test;
  CHECK (create ("test.txt", sizeof sample - 1), "create \"test.txt\"");
  CHECK ((handle = open ("test.txt")) > 1, "open \"test.txt\"");
  temp = write (handle, test, sizeof sample - 1);
  fail ("write() should have returned exit(-1)");
}
