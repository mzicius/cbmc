int main(void){
    int x = 1001;
    int y = -100;

    __CPROVER_assume(x>=y);

    return x;
}