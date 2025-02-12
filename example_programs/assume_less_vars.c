int main(void){
    int x = 7;
    int y = 2;

    __CPROVER_assume(x<y);

    return x;
}