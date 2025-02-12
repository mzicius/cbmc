int main(void){
    int x = 3;
    int y = 9;

    __CPROVER_assume(x<y);

    return x;
}