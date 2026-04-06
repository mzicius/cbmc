int main(){
    int i;
    int c;
    __CPROVER_assume(c >= 0);
    i = 0;
    while(i<c){
        __CPROVER_assert(i<c,"i<c");
        i = i + 1;
        __CPROVER_assert(i<=c,"i<=c");
    }
    __CPROVER_assert(i>=c,"i>=c");
    //implied by previous two asserts
    __CPROVER_assert(i==c,"i==c");

}