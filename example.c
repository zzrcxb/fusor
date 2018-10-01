//
// Created by neil on 7/19/18.
//

#include <stdio.h>
#include <stdlib.h>


int main(int argc, char **argv) {
  int a[5] = {1,2,3,4,5};
  int index = argv[1][0] - '0';

  switch (index) {
    case 0:
      return 1;
    case 1:
      return 2;
    case 2:
      return 3;
    case 3:
      return 4;
  }

  return 0;
}
