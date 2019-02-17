#include <stdio.h>

int test(char* str) {
  char res = str[0] - '0';
  
  switch (res) {
    case 0: printf("0\n"); break;
    case 1: printf("1\n"); break;
    case 2: printf("2\n"); break;
    default: printf("default\n"); break;
  }
}


int main(int argc, char** argv) {
  return test(argv[1]);
}
