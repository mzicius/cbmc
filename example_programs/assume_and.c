int main(void){
    int x = 6000;
    int y = 1000;

    __CPROVER_assume(x>0 && y>0);

    return x;
}