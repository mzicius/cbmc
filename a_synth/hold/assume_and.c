int main(void){
    
    int x = 6000;
    int y = 1000;

    __CPROVER_assert(x<=6000,"greater");

    return x;
}