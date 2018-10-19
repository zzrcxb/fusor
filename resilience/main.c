#include <stdio.h>

int main(int argc, char**argv){
    char i = ((127) & (argv[1][0] - '0')) % 10;
    char a[] = {3,4,5,6,7,1,2,8,9,0};
    char b[] = {9,5,6,0,1,2,3,4,8,7};
    char c[] = {1, 2, 3, 4, 5, 6, 8, 9, 6, 0};
    char d[] = {1, 3, 2, 4, 5, 6, 7, 8, 9, 0};

    // if (b[a[i]] == 5 && c[a[i]] == 2 && d[a[i]] == 3) {
    if (b[a[i]] == 5) {
        printf("Type II opaque predicate should be satisfiable\n");
    }
    else {
        printf("Type I opaque predicate should not be satisfiable\n");
    }
}
