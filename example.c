//
// Created by neil on 7/19/18.
//

#include <stdio.h>
#include <stdlib.h>



int main(int argc, char **argv) {
  int a[5] = {1,2,3,4,5};
  int index = argv[1][0] - '0';

  printf("%d\n", a[index]);

  return 0;
}
