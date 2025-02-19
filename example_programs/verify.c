int main(void){
    int x = 3;
    int y = 3;

    __CPROVER_assume(x==y);
    assert(x>y);

    return x;
}