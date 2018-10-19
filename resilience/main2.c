int test(i){
    int j = abs(i%11);
    int a[] = {(i+5)%11,(i+6)%11,(i+7)%11,(i+8)%11,(i+9)%11,(i+10)%11,i%11,(i+1)%11,(i+2)%11,(i+3)%11,(i+4)%11};
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
