int main(){
int i;
int c;
__CPROVER_assume(c>=0);

  for(i = 0; i < c; ++i)
  {
  __CPROVER_assert(i<c,"i is less");
  }
    __CPROVER_assert(i>=c,"i is goeq");
        __CPROVER_assert(i==c,"i is eq c");

}