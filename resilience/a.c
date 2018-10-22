int test(i){
    int j = abs(i%10);
    int a[][10] = 
{
{7, 3, 5, 8, 6, 9, 0, 4, 2, 1, },
{6, 9, 3, 2, 5, 0, 1, 4, 7, 8, },
{8, 6, 1, 0, 4, 3, 2, 7, 5, 9, },
{0, 9, 3, 1, 8, 7, 2, 5, 4, 6, },
{9, 5, 0, 8, 4, 3, 7, 2, 6, 1, },
{3, 4, 7, 2, 0, 1, 8, 6, 5, 9, },
{3, 8, 4, 0, 6, 5, 1, 7, 9, 2, },
{6, 9, 0, 8, 7, 2, 1, 5, 3, 4, },
};
    int* p = &a[2][a[1][a[0][j]]];
    int* q = &a[7][a[6][a[5][a[4][a[3][*p]]]]];
    printf("p = %d\n", *p);
    if (*p == *q){
        printf("Flag captured! Type II opaque predicate should be satisfiable\n");
        return 1;
    }
    return 0;
}
int main(int argc, char** argv) {
    return test(atoi(argv[1]));
}
