//
// Created by neil on 7/19/18.
//

#include <stdio.h>
#include <stdlib.h>
//#include "example.h"


int main(int argc, char **argv) {
  int test = argv[1][0] - '0';

  if (test == 5)
    return 0;
  else
    return 1;
}