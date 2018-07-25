//
// Created by neil on 7/19/18.
//

#include <stdio.h>
#include "example.h"


int main(int argc, char **argv) {
  int test = argv[1][0] - '0';
  char test2 = argv[1][0];

  fusor_symvar(&test);
  fusor_symvar(&test2);

  if (test > 10) {
    printf("Yeah! %d\n", test);
    return 1;
  }
  else {
    printf("Hmmm! %d\n", test);
    return 0;
  }
}