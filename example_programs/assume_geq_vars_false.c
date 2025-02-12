int main(void){
    int x = 100;
    int y = 150;

    __CPROVER_assume(x>=y);

    return x;
}