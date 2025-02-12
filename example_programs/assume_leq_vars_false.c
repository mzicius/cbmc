int main(void){
    int x = 11;
    int y = 13;

    __CPROVER_assume(y<=x);

    return x;
}