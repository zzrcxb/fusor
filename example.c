//
// Created by neil on 7/19/18.
//

#include <stdio.h>
#include "example.h"


int main(int argc, char **argv) {
  int test = argv[1][0] - '0';
  char test2 = argv[1][0];
  int t3[5] = {0, };
  double t4 = 3.0;

  t3[2] = 5;

  fusor_symvar(&test);
  fusor_symvar(&test2);
  fusor_symvar(&t4);

  if (test > 10) {
    printf("Yeah! %d\n", test);
    return 1;
  }
  else {
    printf("Hmmm! %d\n", test);
    return 0;
  }
}