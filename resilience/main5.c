int test(i){
    int j = abs(i%11);
    int a[][] = {{0, 2, 4, 6, 8, 1, 3, 5, 7, 9},
                 {1, 3, 5, 7, 9, 0, 2, 4, 6, 8},
                 {0, 2, 4, 6, 8, 1, 3, 5, 7, 9},
};
    int* p = &a[a[a[j]]];
    int* q = &a[a[a[a[a[*p]]]]];
    
    printf("p = %d\n", *p);
    if (p == q){
        printf("Flag captured! Type II opaque predicate should be satisfiable\n");
        return 1;
    }
    return 0;
}

int main(int argc, char** argv) {
  return test(argv[1][0] - '0');
}
