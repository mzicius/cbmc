int main(void){
    int x = 2;

    __CPROVER_assume(x<7);

    return x;
}