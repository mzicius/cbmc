int main(void){
    int x = 3;
    int y = 7;

    __CPROVER_assume(x==y);

    return x;
}