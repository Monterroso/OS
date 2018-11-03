/* Tries to seek to a position in a file (test.txt) and uses tell to identify
the position. Based off write-normal */

#include <syscall.h>
#include "tests/userprog/sample.inc"
#include "tests/lib.h"
#include "tests/main.h"

void
test_main (void)
{
  int handle, position, tellpos;

  CHECK (create ("test.txt", sizeof sample - 1), "create \"test.txt\"");
  CHECK ((handle = open ("test.txt")) > 1, "open \"test.txt\"");

  position = 5;
  seek(handle, position);
  tellpos = tell(handle);
  
  if (tellpos != position)
    fail ("tell() returned %d instead of %zu", tellpos, position);
}
