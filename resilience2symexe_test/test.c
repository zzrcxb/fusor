#include <stdio.h>

int main(int argc, char**argv){
    int i = (127 & argv[1][0]) % 10;
    int a[] = {1, 2, 3};

    if (a[i] == 3){
        printf("Type I opaque predicate should not be satisfiable\n");
    }
    else {
        printf("Type II opaque predicate should be satisfiable\n");
    }
}
