/* Tests that filesize works as intended, based off of write-normal */

#include <syscall.h>
#include "tests/userprog/sample.inc"
#include "tests/lib.h"
#include "tests/main.h"

void
test_main (void)
{
  int handle, size, temp;

  CHECK (create ("test.txt", sizeof sample - 1), "create \"test.txt\"");
  CHECK ((handle = open ("test.txt")) > 1, "open \"test.txt\"");
  temp = write (handle, sample, sizeof sample - 1);
  size = filesize(handle);
  if (size != temp) {
    fail ("filesize() returned %d instead of %zu", size, sizeof sample - 1);
  }
}
