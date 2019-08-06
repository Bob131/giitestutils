#include "gtu.h"

int main (int argc, char *argv[]) {
  GtuTestSuite* suite;
  gtu_init (argv, argc);
  suite = gtu_test_suite_new ("test");
  return gtu_test_suite_run (suite);
}
