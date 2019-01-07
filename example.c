#include <stdio.h>

int test(char* str) {
  char res = str[0] - '0';
  char test[20] = "test string";
  char *br1 = "ABCThis is branch I";

  if (res > 5) {
    printf("%s\n", br1 + 3);
    res = 3;
    test[1] = 'c';
  }
  else {
    printf("This is branch II\n");
    res = 0;
  }
  
  printf("%d\n%s\n", res, test);
  return res;
}


int main(int argc, char** argv) {
  return test(argv[1]);
}
