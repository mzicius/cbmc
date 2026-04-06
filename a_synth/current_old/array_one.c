//array_doub_access_init_const.c
//unreach_call: true 
//scenario: iterate over large loop bound
int main()
{
  int i;
  int N=4;
  int a[22];

  for(i=0;i<=N;i++) {
    a[i]=7;
  }

  __CPROVER_assert(a[4]>4,"array element is non-negative");
  
  return 0;
}
