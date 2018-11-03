/* Tries to seek to a position in a file (test.txt) and uses tell to identify
the position. Based off write-normal */

#include <syscall.h>
#include "tests/userprog/sample.inc"
#include "tests/lib.h"
#include "tests/main.h"

int
main (int argc, char * argv[] UNUSED)
{
  test_name = "excess-stack";

  msg("Argument base: %p", &argc);
  return 0;
}
