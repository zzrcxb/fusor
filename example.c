//
// Created by neil on 7/19/18.
//

#include <stdio.h>
#include <stdlib.h>

typedef struct {
  int t1;
  char t2, t3;
} TS;

int test(char a1, TS a2) {
  char index1, index2;
  index2 = *((char*)(&a2));
  index1 = index2 % 8;
  printf("%d\n", index2);
}

int main(int argc, char **argv) {
  TS t;
  t.t1 = argc;
  t.t2 = 0;
  t.t3 = 1;
  test(-34, t);
  return 0;
}
