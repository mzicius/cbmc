int main(void){
    int x = 3;
    int y = 4;

    __CPROVER_assume(x<=y);

    return x;
}