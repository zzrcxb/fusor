#include <stdio.h>

int test(char* str) {
  char a[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 0};
  char b[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 0};
  char c[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 0};
  char d[] = {1, 3, 2, 4, 5, 6, 7, 8, 9, 0};
  char res = str[0] - '0';
  
  // printf("dbg: %c", res);

  if (res > 5) {
    printf("This is branch I\n");
    return 3;
  }
  else {
    printf("This is branch II\n");
    return 0;
  }
}


int main(int argc, char** argv) {
  return test(argv[1]);
}
